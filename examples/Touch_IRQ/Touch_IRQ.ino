/* 
 * Touch_IRQ.ino
 * 
 * This example shows how to get the number of points and the coordinates of the first touch points detected by the touch controller using interrupts.
 * 
 * The setup() function initializes the serial communication and the touch controller. Whenever a touch event is detcted, the gigaTouchHandler function is called that prints the number of points and first touch co-ordinates to the Serial Monitor. The loop() function is empty because the touch controller is configured to work with interrupts. 
 * 
 * For the polling version of this example, see Touch_Polling.ino
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

void gigaTouchHandler(uint8_t contacts, GDTpoint_t* points) {
  Serial.print("Contacts: ");
  Serial.println(contacts);

  if (contacts > 0) {
    /* First touch point */
    Serial.print(points[0].x);
    Serial.print(" ");
    Serial.println(points[0].y);
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial) {}

  if (touchDetector.begin()) {
    Serial.println("Touch controller init - OK");
  } else {
    Serial.println("Touch controller init - FAILED");
    while(1) ;
  }

  touchDetector.onDetect(gigaTouchHandler);
}

void loop() { }