/*
  GigaDisplay_touch

  created DD MMM YYYY
  by Leonardo Cavagnis
*/

#include "Arduino_DisplayShieldTouch.h"
#include "pinDefinitions.h"

#define Wire          Wire1
#define INT_PIN       PinNameToIndex(PI_1)
#define RST_PIN       PinNameToIndex(PI_2)
#define WIRE_ADDR     GT911_I2C_ADDR_BA_BB

Arduino_DisplayShieldTouch touch(Wire, INT_PIN, RST_PIN, WIRE_ADDR);

void setup() { 
  Serial.begin(115200);
  while(!Serial) {}

  Wire.setClock(400000);
  Wire.begin();

  if (touch.begin()) {
    Serial.println("GT911 init OK");
  } else {
    Serial.println("GT011 init FAILED");
  }

  Serial.print("Check ACK on addr request on 0x");
  Serial.println(WIRE_ADDR, HEX);
  
  Wire.beginTransmission(WIRE_ADDR);
  Wire.write(0);
  int error = Wire.endTransmission();
  if (error == 0) {
    Serial.println("SUCCESS");
  } else {
    Serial.print("ERROR #");
    Serial.println(error);
  }

}

void loop() {
    
}



