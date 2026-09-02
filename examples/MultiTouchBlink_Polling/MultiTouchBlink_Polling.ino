/* 
 * MultiTouchBlink_Polling.ino
 * 
 * This example shows how to visualise the number of touch points, using the RGB LED. 
 * 
 * On each iteration of the loop, the touch points are obtained from the GT911 controller. If the touch points exist, then a message
 * is sent to the Serial, and the LED is blinked the corresponding number of times. A 1 second delay is given after a touch is detected, to
 * act as a basic debounce utility.  
 *  
 * For the interrupt version of this example, see MultiTouchBlink_IRQ.ino
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

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (touchDetector.begin()) {
    Serial.println("Touch controller init - OK");
  } else {
    Serial.println("Touch controller init - FAILED");
    while (1) ;
  }
}

void loop() {
  uint8_t contacts;
  GDTpoint_t points[5];
  contactPoints = 0;

  contactPoints = touchDetector.getTouchPoints(points);

  if (contactPoints > 0) {
    Serial.println("Contact Detected! Will Blink " + String(contactPoints) + " times!");
    for (int i = 1; i <= contactPoints; i++) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(250);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(250);
    }

    delay(1000);
  }

  delay(1);
}