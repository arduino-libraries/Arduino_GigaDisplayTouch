/*
  GigaDisplay_touch

  created DD MMM YYYY
  by Leonardo Cavagnis
*/

#include "Arduino_GigaDisplayTouch.h"

#define TOUCH_MODE    1 /* 1: Interrupt, 0: Polling */

Arduino_GigaDisplayTouch touch;

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

  if (touch.begin()) {
    Serial.print("Touch controller init - OK");
  } else {
    Serial.print("Touch controller init - FAILED");
    while(1) ;
  }
#if TOUCH_MODE
  touch.attach(gigaTouchHandler);
#endif
}

void loop() {
  #if TOUCH_MODE
    touch.detect();
  #else
    uint8_t contacts;
    GDTpoint_t points[5];
    if(touch.detect(contacts, points)) {
      Serial.print("Contacts: ");
      Serial.println(contacts);

      for (uint8_t i = 0; i < contacts; i++) {
        Serial.print(points[i].x);
        Serial.print(" ");
        Serial.println(points[i].y);
      }
    }
  #endif

  delay(1);
}



