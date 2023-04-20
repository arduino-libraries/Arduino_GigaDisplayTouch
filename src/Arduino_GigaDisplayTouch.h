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

/* Exported defines ----------------------------------------------------------*/
#define GT911_I2C_ADDR_28_29    (((0x28 >> 1) & 0x7F) | 0x80) // 0x28/0x29 - 0x14 (7bit)
#define GT911_I2C_ADDR_BA_BB    (((0xBA >> 1) & 0x7F) | 0x80) // 0xBA/0xBB - 0x5D (7bit)

#define GT911_CONTACT_SIZE      8
#define GT911_MAX_CONTACTS      5

/* Exported types ------------------------------------------------------------*/
typedef struct GDTcoord_s GDTcoord_t;

/* Exported enumeration ------------------------------------------------------*/

/* Exported struct -----------------------------------------------------------*/
struct GDTcoord_s {
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
      Arduino_GigaDisplayTouch(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr);
      ~Arduino_GigaDisplayTouch();

      bool begin();
      void end();

      void detect();
      void attachTouchHandler(void (*handler)(uint8_t, GDTcoord_t*));
  private:
      TwoWire&      _wire;
      uint8_t       _intPin;
      uint8_t       _rstPin;
      uint8_t       _addr;
      GDTcoord_t    _coords[GT911_MAX_CONTACTS];
      void          (*_gt911TouchHandler)(uint8_t, GDTcoord_t*);
      
      uint8_t   _gt911WriteOp(uint16_t reg, uint8_t data);
      uint8_t   _gt911WriteBytesOp(uint16_t reg, uint8_t * data, uint8_t len);
      uint8_t   _gt911ReadOp(uint16_t reg, uint8_t * data, uint8_t len);
      void      _gt911onIrq();
      uint8_t   _gt911ReadInputCoord(uint8_t * pointsbuf, uint8_t& contacts);
};

#endif /* _ARDUINO_GIGADISPLAYTOUCH_H */