/* 
 * MultiTouchBlink_IRQ.ino
 * 
 * This example shows how to visualise the number of touch points, using the RGB LED. 
 * 
 * Whenever a touch event is detected, gigaTouchHandler() records the number of touches and blinks the RGB LED accordingly.
 * The event is also detected and sent via Serial. The touch handler is detached, blink is performed and then re-attached.
 *  
 * For the polling version of this example, see MultiTouchBlink_IRQ.ino
 * 
 * Instructions:
 * 1. Connect your GIGA Display Shield (ASX00039) to a GIGA R1 WiFi (ABX00063) board.
 * 2. Upload this sketch to your board.
 * 3. Open the Serial Monitor.
 * 4. Touch the screen with your finger(s).
 * 5. The LED will blink, based on how many touch contact are recorded. Note that fingers need to be placed all at the same time. 
 * 
 * Initial author: Ali Jahangiri @aliphys
 * Created: 12 June 2024
 */

#include "Arduino_GigaDisplayTouch.h"

Arduino_GigaDisplayTouch touchDetector;

int contactPoints = 0;

void gigaTouchHandler(uint8_t contacts, GDTpoint_t* points) {
  contactPoints = contacts;
  touchDetector.end();  // close the touch controller
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (touchDetector.begin()) {
    Serial.println("Touch controller init - OK");
  } else {
    Serial.println("Touch controller init - FAILED");
    while (1);
  }

  touchDetector.onDetect(gigaTouchHandler);
}

void loop() {
  if (contactPoints > 0) {
    Serial.println("Contact Detected! Will Blink " + String(contactPoints) + " times!");
    for (int i = 0; i < contactPoints; i++) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
    }
    contactPoints = 0;
    touchDetector.begin();
    delay(100);
    touchDetector.onDetect(gigaTouchHandler);
  }
}