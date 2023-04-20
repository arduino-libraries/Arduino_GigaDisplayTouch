/*
  GigaDisplay_touch

  created DD MMM YYYY
  by Leonardo Cavagnis
*/

#include "Arduino_GigaDisplayTouch.h"
#include "pinDefinitions.h"

#define Wire          Wire1
#define INT_PIN       PinNameToIndex(PI_1)
#define RST_PIN       PinNameToIndex(PI_2)
#define WIRE_ADDR     GT911_I2C_ADDR_BA_BB

#define TOUCH_MODE    1 /* 1: Interrupt, 0: Polling */

Arduino_GigaDisplayTouch touch(Wire, INT_PIN, RST_PIN, WIRE_ADDR);

void gigaTouchHandler(uint8_t contacts, GDTpoint_t* points) {
  Serial.print("Contacts: ");
  Serial.println(contacts);

  for (uint8_t i = 0; i < contacts; i++) {
    Serial.print(points[i].x);
    Serial.print(" ");
    Serial.println(points[i].y);
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  Wire.setClock(400000); /* maximum transmission rate of 400K bps */
  Wire.begin();

  Serial.print("Touch controller init ");
  if (touch.begin()) {
    Serial.println("SUCCESS");
  } else {
    Serial.println("FAILED");
    while(1) ;
  }
#if TOUCH_MODE
  touch.attachTouchHandler(gigaTouchHandler);
#endif
}

void loop() {
  #if TOUCH_MODE
    touch.detect();
  #else
    uint8_t contacts;
    GDTpoint_t points[5];
    if(touch.detect(contacts, points)) {
      Serial.println("Contacts!");
    }
  #endif

  delay(1);
}



