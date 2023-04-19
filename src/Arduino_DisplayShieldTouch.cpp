/**
  ******************************************************************************
  * @file    Arduino_DisplayShieldTouch.cpp
  * @author  Leonardo Cavagnis
  * @version 
  * @date    
  * @brief
  ******************************************************************************
  */

 /* Includes -----------------------------------------------------------------*/
#include "Arduino_DisplayShieldTouch.h"

/* Private defines -----------------------------------------------------------*/
#define GT911_READ_COORD_ADDR   0x814E
#define GT911_REG_CONFIG_DATA   0x8047

/* Private variables ---------------------------------------------------------*/
volatile bool _gt911DataReadyIrq = false;

/* Private function prototypes -----------------------------------------------*/
void _gt911_irqHandler();

/* Functions -----------------------------------------------------------------*/
Arduino_DisplayShieldTouch::Arduino_DisplayShieldTouch(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr)
: _wire{wire}, _intPin{intPin}, _rstPin{rstPin}, _addr{addr} 
{ }

Arduino_DisplayShieldTouch::~Arduino_DisplayShieldTouch() 
{ }

bool Arduino_DisplayShieldTouch::begin() {
    // Take chip some time to start
    delay(300);
    pinMode(_rstPin, OUTPUT);
    digitalWrite(_rstPin, LOW);
    /* begin select I2C slave addr */
    /* T2: > 10ms */
    delay(11);
    /* HIGH: 0x28/0x29 (0x14 7bit), LOW: 0xBA/0xBB (0x5D 7bit) */
    digitalWrite(_intPin, LOW); //@FIXME
    /* T3: > 100us */
    delayMicroseconds(110);
    pinMode(_rstPin, INPUT);
    /* T4: > 5ms */
    delay(6);
    digitalWrite(_intPin, LOW);
    /* end select I2C slave addr */
    /* T5: 50ms */
    delay(51);
    pinMode(_intPin, INPUT); // INT pin has no pullups so simple set to floating input
    attachInterrupt(_intPin, _gt911_irqHandler, RISING);
    delay(200);

    /* Test communication */
    uint8_t testByte;
    uint8_t error = _gt911ReadOp(GT911_REG_CONFIG_DATA,  &testByte, 1);

    return (error == 0);
}

void Arduino_DisplayShieldTouch::end() 
{ }

void Arduino_DisplayShieldTouch::detect() {
  bool irq = _gt911DataReadyIrq;
  _gt911DataReadyIrq = false;

  if (irq) {
    _gt911onIrq();
  }
}

uint8_t Arduino_DisplayShieldTouch::_gt911WriteOp(uint16_t reg, uint8_t data) {
    _gt911WriteBytesOp(reg, &data, 1);
}

uint8_t Arduino_DisplayShieldTouch::_gt911WriteBytesOp(uint16_t reg, uint8_t * data, uint8_t len) {
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

uint8_t Arduino_DisplayShieldTouch::_gt911ReadOp(uint16_t reg, uint8_t * data, uint8_t len) {
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

void Arduino_DisplayShieldTouch::_gt911onIrq() {
    uint8_t contacts;
    uint8_t rawdata[GT911_MAX_CONTACTS * GT911_CONTACT_SIZE]; //points buffer
    uint8_t error;

    error = _gt911ReadInputCoord(rawdata, contacts);

    if (error) {
        return;
    }

    if (contacts > 0) {
        _points[0].trackId  = rawdata[1];	    
        _points[0].x        = ((uint16_t)rawdata[3] << 8) + rawdata[2];
        _points[0].y        = ((uint16_t)rawdata[5] << 8) + rawdata[4];
        _points[0].area     = ((uint16_t)rawdata[7] << 8) + rawdata[6];

        _points[1].trackId  = rawdata[9];
        _points[1].x        = ((uint16_t)rawdata[11] << 8) + rawdata[10];
        _points[1].y        = ((uint16_t)rawdata[13] << 8) + rawdata[12];
        _points[1].area     = ((uint16_t)rawdata[15] << 8) + rawdata[14];

        _points[2].trackId  = rawdata[17];
        _points[2].x        = ((uint16_t)rawdata[19] << 8) + rawdata[18];
        _points[2].y        = ((uint16_t)rawdata[21] << 8) + rawdata[20];
        _points[2].area     = ((uint16_t)rawdata[23] << 8) + rawdata[22];

        _points[3].trackId  = rawdata[25];
        _points[3].x        = ((uint16_t)rawdata[27] << 8) + rawdata[26];
        _points[3].y        = ((uint16_t)rawdata[29] << 8) + rawdata[28];
        _points[3].area     = ((uint16_t)rawdata[31] << 8) + rawdata[30];

        _points[4].trackId  = rawdata[33];
        _points[4].x        = ((uint16_t)rawdata[35] << 8) + rawdata[34];
        _points[4].y        = ((uint16_t)rawdata[37] << 8) + rawdata[36]; 
        _points[4].area     = ((uint16_t)rawdata[39] << 8) + rawdata[38];
        
        //@TODO: touchHandler(contacts, points);
    }
    _gt911WriteOp(GT911_READ_COORD_ADDR, 0);
}

uint8_t Arduino_DisplayShieldTouch::_gt911ReadInputCoord(uint8_t * pointsbuf, uint8_t& contacts) {
    uint8_t error;
    
    contacts = 0;
    error    = _gt911ReadOp(GT911_READ_COORD_ADDR, pointsbuf, GT911_CONTACT_SIZE * GT911_MAX_CONTACTS);

    if (error) {
        return 1;
    }
    
    if (!(pointsbuf[0] & 0x80)) {	  
        return 2;
    }

    contacts = pointsbuf[0] & 0xF;
    return 0;
}

void _gt911_irqHandler() {
  noInterrupts();
  _gt911DataReadyIrq = true;
  interrupts();
}

/**** END OF FILE ****/