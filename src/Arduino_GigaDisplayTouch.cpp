/**
  ******************************************************************************
  * @file    Arduino_GigaDisplayTouch.cpp
  * @author  Leonardo Cavagnis
  * @version 
  * @date    
  * @brief
  ******************************************************************************
  */

 /* Includes -----------------------------------------------------------------*/
#include "Arduino_GigaDisplayTouch.h"

/* Private defines -----------------------------------------------------------*/
#define GT911_READ_COORD_ADDR   0x814E
#define GT911_REG_CONFIG_DATA   0x8047

/* Private variables ---------------------------------------------------------*/
volatile bool _gt911DataReadyIrq = false;

/* Private function prototypes -----------------------------------------------*/
void _gt911_irqHandler();

/* Functions -----------------------------------------------------------------*/
Arduino_GigaDisplayTouch::Arduino_GigaDisplayTouch(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr)
: _wire{wire}, _intPin{intPin}, _rstPin{rstPin}, _addr{addr} 
{ }

Arduino_GigaDisplayTouch::~Arduino_GigaDisplayTouch() 
{ }

bool Arduino_GigaDisplayTouch::begin() {
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

    _gt911DataReadyIrq = false;
    attachInterrupt(_intPin, _gt911_irqHandler, RISING);

    /* GT911 test communication */
    uint8_t testByte;
    uint8_t error = _gt911ReadOp(GT911_REG_CONFIG_DATA,  &testByte, 1);

    return (error == 0);
}

void Arduino_GigaDisplayTouch::end() 
{ }

void Arduino_GigaDisplayTouch::detect() {
  if (_gt911DataReadyIrq) {
    _gt911onIrq();
    _gt911DataReadyIrq = false;
  }
}

uint8_t Arduino_GigaDisplayTouch::_gt911WriteOp(uint16_t reg, uint8_t data) {
    _gt911WriteBytesOp(reg, &data, 1);
}

uint8_t Arduino_GigaDisplayTouch::_gt911WriteBytesOp(uint16_t reg, uint8_t * data, uint8_t len) {
    uint8_t status = 0;

    _wire.beginTransmission(_addr);
    _wire.write(reg >> 8);
    _wire.write(reg & 0xFF);

    for (uint8_t i = 0; i < len; i++) {
        _wire.write(data[i]);
    }

    status = _wire.endTransmission();

    return status;
}

uint8_t Arduino_GigaDisplayTouch::_gt911ReadOp(uint16_t reg, uint8_t * data, uint8_t len) {
    uint8_t status = 0;

    _wire.beginTransmission(_addr);
    _wire.write(reg >> 8);
    _wire.write(reg & 0xFF);
    status = _wire.endTransmission();

    if (status) return status;

    _wire.requestFrom(_addr, len);
    uint8_t index = 0;
    while (_wire.available()) {
        data[index++] = _wire.read();
    }

    if (len == index) return 0; /* Success */
    else return 4; /* Other error */
}

void Arduino_GigaDisplayTouch::_gt911onIrq() {
    uint8_t contacts;
    uint8_t rawcoords[GT911_MAX_CONTACTS * GT911_CONTACT_SIZE];
    uint8_t error;

    error = _gt911ReadInputCoord(rawcoords, contacts);

    if (error) {
        return;
    }

    for (uint8_t i = 0; i < contacts; i++) {
        _coords[i].trackId  = rawcoords[1 + 8*i];	    
        _coords[i].x        = ((uint16_t)rawcoords[3 + 8*i] << 8) + rawcoords[2 + 8*i];
        _coords[i].y        = ((uint16_t)rawcoords[5 + 8*i] << 8) + rawcoords[4 + 8*i];
        _coords[i].area     = ((uint16_t)rawcoords[7 + 8*i] << 8) + rawcoords[6 + 8*i];
    }

    if (contacts > 0) {
        Serial.println("Contacts: ");
        Serial.println(_coords[0].x);
        Serial.println(_coords[0].y);
        //@TODO: touchHandler(contacts, _coords);
    }
    
    _gt911WriteOp(GT911_READ_COORD_ADDR, 0);
}

uint8_t Arduino_GigaDisplayTouch::_gt911ReadInputCoord(uint8_t * pointsbuf, uint8_t& contacts) {
    uint8_t error;
    
    contacts = 0;
    error    = _gt911ReadOp(GT911_READ_COORD_ADDR, pointsbuf, GT911_CONTACT_SIZE * GT911_MAX_CONTACTS);

    if (error) {
        return 1; /* I2C Error */
    }
    
    if (!(pointsbuf[0] & 0x80)) {	  
        return 2;
    }

    contacts = pointsbuf[0] & 0xF;
    return 0; /* Success */
}

void _gt911_irqHandler() {
  noInterrupts();
  _gt911DataReadyIrq = true;
  interrupts();
}

/**** END OF FILE ****/