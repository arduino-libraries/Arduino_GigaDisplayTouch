/*
 * Copyright 2023 Arduino SA
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

/**
 * @file Arduino_GigaDisplayTouch.cpp
 * @author Leonardo Cavagnis
 * @brief Source file for the Arduino Giga Display Touch library.
 */

 /* Includes -----------------------------------------------------------------*/
#include "Arduino_GigaDisplayTouch.h"

/* Private defines -----------------------------------------------------------*/
#define GT911_REG_GESTURE_START_POINT   0x814E
#define GT911_REG_CONFIG_VERSION        0x8047

/* Private variables ---------------------------------------------------------*/
rtos::Thread t;
events::EventQueue queue(32 * EVENTS_EVENT_SIZE);  

/* Private function prototypes -----------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
Arduino_GigaDisplayTouch::Arduino_GigaDisplayTouch(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr)
: _wire{wire}, _intPin{intPin}, _rstPin{rstPin}, _addr{addr}, _irqInt{digitalPinToPinName(intPin)}
{ }

Arduino_GigaDisplayTouch::~Arduino_GigaDisplayTouch() 
{ }

bool Arduino_GigaDisplayTouch::begin() {
    _wire.setClock(400000); /* maximum transmission rate of 400K bps */
    _wire.begin();

    delay(300);

    /* GT911 Power-on timing procedure - Ref. pg10 GT911 Rev09 */
    /** T0: Set output low */
    pinMode(_rstPin, OUTPUT);
    pinMode(_intPin, OUTPUT);
    digitalWrite(_rstPin, LOW);
    digitalWrite(_intPin, LOW);
    /** T1+T2: > 10ms */
    delay(11);
    /** Address selection: high - 0x28/0x29 (0x14 7bit), low - 0xBA/0xBB (0x5D 7bit) */
    digitalWrite(_intPin, (_addr == GT911_I2C_ADDR_28_29));
    /** T7: > 100us */
    delayMicroseconds(110);
    digitalWrite(_rstPin, HIGH);
    /** T8: > 5ms */
    delay(6);
    digitalWrite(_intPin, LOW);
    /** T3: > 50ms */
    delay(51);
    pinMode(_intPin, INPUT);

    _gt911TouchHandler = nullptr;

    /* GT911 test communication */
    uint8_t testByte;
    uint8_t error = _gt911ReadOp(GT911_REG_CONFIG_VERSION,  &testByte, 1);

    return (error == 0);
}

void Arduino_GigaDisplayTouch::end() 
{ }

bool Arduino_GigaDisplayTouch::detect(uint8_t& contacts, GDTpoint_t* points) {
    uint8_t rawpoints[GT911_MAX_CONTACTS * GT911_CONTACT_SIZE];
    uint8_t error;

    error = _gt911ReadInputCoord(rawpoints, contacts);

    if (error) {
        return false;
    }

    for (uint8_t i = 0; i < contacts; i++) {
        points[i].trackId  = rawpoints[1 + 8*i];	    
        points[i].x        = ((uint16_t)rawpoints[3 + 8*i] << 8) + rawpoints[2 + 8*i];
        points[i].y        = ((uint16_t)rawpoints[5 + 8*i] << 8) + rawpoints[4 + 8*i];
        points[i].area     = ((uint16_t)rawpoints[7 + 8*i] << 8) + rawpoints[6 + 8*i];
    }
 
    _gt911WriteOp(GT911_REG_GESTURE_START_POINT, 0); /* Reset buffer status to finish the reading */

    if (contacts > 0)   return true;
    else                return false;
}

void Arduino_GigaDisplayTouch::attach(void (*handler)(uint8_t, GDTpoint_t*)) {
    t.start(callback(&queue, &events::EventQueue::dispatch_forever));
    _irqInt.rise(queue.event(mbed::callback(this, &Arduino_GigaDisplayTouch::_gt911onIrq)));

    _gt911TouchHandler = handler;
}

uint8_t Arduino_GigaDisplayTouch::_gt911WriteOp(uint16_t reg, uint8_t data) {
    uint8_t status = 0;
    status = _gt911WriteBytesOp(reg, &data, 1);

    return status;
}

uint8_t Arduino_GigaDisplayTouch::_gt911WriteBytesOp(uint16_t reg, uint8_t * data, uint8_t len) {
    uint8_t status = 0;

    _wire.beginTransmission(_addr);
    _wire.write(reg >> 8);      /* Register H */
    _wire.write(reg & 0xFF);    /* Register L */

    /* Data [0..n] */
    for (uint8_t i = 0; i < len; i++) {
        _wire.write(data[i]); 
    }

    status = _wire.endTransmission();

    return status;
}

uint8_t Arduino_GigaDisplayTouch::_gt911ReadOp(uint16_t reg, uint8_t * data, uint8_t len) {
    uint8_t status = 0;

    _wire.beginTransmission(_addr);
    _wire.write(reg >> 8);      /* Register H */
    _wire.write(reg & 0xFF);    /* Register L */
    status = _wire.endTransmission();

    if (status) return status;

    _wire.requestFrom(_addr, len);
    uint8_t index = 0;
    /* Data [0..n] */
    while (_wire.available()) {
        data[index++] = _wire.read();
    }

    if (len == index)   return 0;
    else                return 4; /* Other error */
}

void Arduino_GigaDisplayTouch::_gt911onIrq() {
    uint8_t contacts;
    uint8_t rawpoints[GT911_MAX_CONTACTS * GT911_CONTACT_SIZE];
    uint8_t error;

    error = _gt911ReadInputCoord(rawpoints, contacts);

    if (error) {
        return;
    }

    for (uint8_t i = 0; i < contacts; i++) {
        _points[i].trackId  = rawpoints[1 + 8*i];	    
        _points[i].x        = ((uint16_t)rawpoints[3 + 8*i] << 8) + rawpoints[2 + 8*i];
        _points[i].y        = ((uint16_t)rawpoints[5 + 8*i] << 8) + rawpoints[4 + 8*i];
        _points[i].area     = ((uint16_t)rawpoints[7 + 8*i] << 8) + rawpoints[6 + 8*i];
    }

    if (contacts > 0 && _gt911TouchHandler != nullptr) _gt911TouchHandler(contacts, _points);
 
    _gt911WriteOp(GT911_REG_GESTURE_START_POINT, 0); /* Reset buffer status to finish the reading */
}

uint8_t Arduino_GigaDisplayTouch::_gt911ReadInputCoord(uint8_t * pointsbuf, uint8_t& contacts) {
    uint8_t error;
    
    contacts = 0;
    error    = _gt911ReadOp(GT911_REG_GESTURE_START_POINT, pointsbuf, GT911_CONTACT_SIZE * GT911_MAX_CONTACTS);

    if (error) {
        return 1; /* I2C comm error */
    }
    
    if (!(pointsbuf[0] & 0x80)) {	  
        return 2; /* Data buffer not ready */
    }

    contacts = pointsbuf[0] & 0xF;
    return 0;
}

/**** END OF FILE ****/