/*
  Touch_IRQ

  created 03 May 2023
  by Leonardo Cavagnis
*/

#include "Arduino_GigaDisplayTouch.h"

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
    Serial.println("Touch controller init - OK");
  } else {
    Serial.println("Touch controller init - FAILED");
    while(1) ;
  }

  touch.attach(gigaTouchHandler);
}

void loop() { }