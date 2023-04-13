/**
  ******************************************************************************
  * @file    Arduino_DisplayShieldTouch.cpp
  * @author  Leonardo Cavagnis
  * @version 
  * @date    
  * @brief
  ******************************************************************************
  */

 /* Includes ------------------------------------------------------------------*/
#include "Arduino_DisplayShieldTouch.h"

/* Private function prototypes -----------------------------------------------*/
void _gt911_intHandler();

/* Functions -----------------------------------------------------------------*/
Arduino_DisplayShieldTouch::Arduino_DisplayShieldTouch(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr)
: _wire{wire}, _intPin{intPin}, _rstPin{rstPin}, _addr{addr} 
{ }

Arduino_DisplayShieldTouch::~Arduino_DisplayShieldTouch() 
{ }

int Arduino_DisplayShieldTouch::begin() {
    // Take chip some time to start
    delay(300);
    pinMode(_rstPin, OUTPUT);
    digitalWrite(_rstPin, LOW);
    /* begin select I2C slave addr */
    /* T2: > 10ms */
    delay(11);
    pinMode(_intPin, OUTPUT);
    /* HIGH: 0x28/0x29 (0x14 7bit), LOW: 0xBA/0xBB (0x5D 7bit) */
    digitalWrite(_intPin, LOW); //@FIXME: ?
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
    attachInterrupt(_intPin, _gt911_intHandler, RISING);
    delay(200);

    return 1;
}

void _gt911_intHandler() {
    //@TODO
}

/**** END OF FILE ****/