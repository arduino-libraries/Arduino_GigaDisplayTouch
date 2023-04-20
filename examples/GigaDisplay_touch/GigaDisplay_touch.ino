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

Arduino_GigaDisplayTouch touch(Wire, INT_PIN, RST_PIN, WIRE_ADDR);

void gigaTouchHandler(uint8_t contacts, GDTcoord_t* coord) {
  Serial.print("Contacts: ");
  Serial.println(contacts);

  for (uint8_t i = 0; i < contacts; i++) {
    Serial.print(coord[i].x);
    Serial.print(" ");
    Serial.println(coord[i].y);
  }
}

void setup() { 
  Serial.begin(115200);
  while(!Serial) {}

  Wire.setClock(400000);
  Wire.begin();

  Serial.print("Touch controller init ");
  if (touch.begin()) {
    Serial.println("SUCCESS");
  } else {
    Serial.println("FAILED");
    while(1) ;
  }

  touch.attachTouchHandler(gigaTouchHandler);
}

void loop() {
  touch.detect();
  delay(1);
}



