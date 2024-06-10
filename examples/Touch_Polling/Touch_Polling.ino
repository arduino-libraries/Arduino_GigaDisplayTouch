/* 
 * Touch_Polling.ino
 * 
 * This example shows how to get the number of points and the coordinates of the first touch points detected by the touch controller using polling.
 * 
 * The setup() function initializes the serial communication and the touch controller. The loop() function continuously checks to see if a touch event is detected. Whenever a touch event is detcted, that prints the number of points and first touch co-ordinates to the Serial Monitor.
 * 
 * For the interrupt version of this example, see Touch_IRQ.ino
 * 
 * Instructions:
 * 1. Connect your GIGA Display Shield (ASX00039) to a GIGA R1 WiFi (ABX00063) board .
 * 2. Upload this sketch to your board.
 * 3. Open the Serial Monitor.
 * 4. Touch the screen with your finger(s) and view the coordinates printed on the Serial Monitor.
 * 
 * Initial author: Leonardo Cavagnis @leonardocavagnis
 * Created: 03 May 2023
 */

#include "Arduino_GigaDisplayTouch.h"

Arduino_GigaDisplayTouch touchDetector;

void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  if (touchDetector.begin()) {
    Serial.print("Touch controller init - OK");
  } else {
    Serial.print("Touch controller init - FAILED");
    while(1) ;
  }
}

void loop() {
  uint8_t contacts;
  GDTpoint_t points[5];
  
  contacts = touchDetector.getTouchPoints(points);

  if (contacts > 0) {
    Serial.print("Contacts: ");
    Serial.println(contacts);

    for (uint8_t i = 0; i < contacts; i++) {
      Serial.print(points[i].x);
      Serial.print(" ");
      Serial.println(points[i].y);
    }
  }

  delay(1);
}