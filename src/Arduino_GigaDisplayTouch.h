/**
  ******************************************************************************
  * @file    Arduino_GigaDisplayTouch.h
  * @author  Leonardo Cavagnis
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  */

#ifndef _ARDUINO_GIGADISPLAYTOUCH_H
#define _ARDUINO_GIGADISPLAYTOUCH_H

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include "Wire.h"
#include "mbed.h"
#include "pinDefinitions.h"

/* Exported defines ----------------------------------------------------------*/
#define GT911_I2C_ADDR_BA_BB    (0x5D | 0x80)  // 0xBA/0xBB - 0x5D (7bit address)
#define GT911_I2C_ADDR_28_29    (0x14 | 0x80)  // 0x28/0x29 - 0x14 (7bit address)

#define GT911_CONTACT_SIZE      8
#define GT911_MAX_CONTACTS      5

/* Exported types ------------------------------------------------------------*/
typedef struct GDTpoint_s GDTpoint_t;

/* Exported enumeration ------------------------------------------------------*/

/* Exported struct -----------------------------------------------------------*/
struct GDTpoint_s {
  // 0x814F-0x8156, ... 0x8176 (5 points) 
  uint8_t trackId;
  uint16_t x;
  uint16_t y;
  uint16_t area;
  uint8_t reserved;
};

/* Class ----------------------------------------------------------------------*/
class Arduino_GigaDisplayTouch {
  public:  
    #if defined(ARDUINO_GIGA)
      Arduino_GigaDisplayTouch(TwoWire& wire  = Wire1, 
                               uint8_t intPin = PinNameToIndex(PI_1),
                               uint8_t rstPin = PinNameToIndex(PI_2), 
                               uint8_t addr   = GT911_I2C_ADDR_BA_BB);
    #elif defined(ARDUINO_PORTENTA_H7_M7)
      Arduino_GigaDisplayTouch(TwoWire& wire  = Wire, 
                               uint8_t intPin = PinNameToIndex(PD_4),
                               uint8_t rstPin = PinNameToIndex(PD_5), 
                               uint8_t addr   = GT911_I2C_ADDR_BA_BB);
    #else 
      Arduino_GigaDisplayTouch(TwoWire& wire, 
                               uint8_t intPin, 
                               uint8_t rstPin, 
                               uint8_t addr);
    #endif
      ~Arduino_GigaDisplayTouch();

      bool begin();
      void end();

      bool detect(uint8_t& contacts, GDTpoint_t* points);
      void attach(void (*handler)(uint8_t, GDTpoint_t*));
  private:
      TwoWire&          _wire;
      uint8_t           _intPin;
      mbed::InterruptIn _irqInt; 
      uint8_t           _rstPin;
      uint8_t           _addr;
      GDTpoint_t        _points[GT911_MAX_CONTACTS];
      void              (*_gt911TouchHandler)(uint8_t, GDTpoint_t*);

      uint8_t   _gt911WriteOp(uint16_t reg, uint8_t data);
      uint8_t   _gt911WriteBytesOp(uint16_t reg, uint8_t * data, uint8_t len);
      uint8_t   _gt911ReadOp(uint16_t reg, uint8_t * data, uint8_t len);
      void      _gt911onIrq();
      uint8_t   _gt911ReadInputCoord(uint8_t * pointsbuf, uint8_t& contacts);
};

#endif /* _ARDUINO_GIGADISPLAYTOUCH_H */