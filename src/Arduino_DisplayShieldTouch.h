/**
  ******************************************************************************
  * @file    Arduino_DisplayShieldTouch.h
  * @author  Leonardo Cavagnis
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  */

#ifndef _ARDUINO_DISPLAYSHIELDTOUCH_H
#define _ARDUINO_DISPLAYSHIELDTOUCH_H

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include "Wire.h"

/* Exported defines ----------------------------------------------------------*/
#define GT911_I2C_ADDR_28_29  (((0x28 >> 1) & 0x7F) | 0x80) // 0x28/0x29 - 0x14 (7bit)
#define GT911_I2C_ADDR_BA_BB  (((0xBA >> 1) & 0x7F) | 0x80) // 0xBA/0xBB - 0x5D (7bit)

/* Exported enumeration ------------------------------------------------------*/

/* Class ----------------------------------------------------------------------*/
class Arduino_DisplayShieldTouch {
    public:
        Arduino_DisplayShieldTouch(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr);
        virtual ~Arduino_DisplayShieldTouch();

        int begin();
        void end();
    private:
        TwoWire&    _wire;
        uint8_t     _intPin;
        uint8_t     _rstPin;
        uint8_t     _addr;
};

#endif /* _ARDUINO_DISPLAYSHIELDTOUCH_H */