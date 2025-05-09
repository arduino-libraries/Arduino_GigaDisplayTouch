/* 
 * Section_Polling.ino
 * 
 * This example shows how to use touch screen inputs to control RGB LED states based on different regions of the screen, via polling.
 * 
 * The loop() function continuously checks to see if a touch event is detected.
 * A millis() based debounce is implemented to prevent multiple touch events. Based on the region touched, the LED status is changed.
 * This is also reported over the Serial Monitor. Since the GIGA R1 WiFi RGB LEDs are active low (common annode),
 * the LED status is inverted.
 *  
 * For the interrupt version of this example, see Section_IRQ.ino
 * 
 * Instructions:
 * 1. Connect your GIGA Display Shield (ASX00039) to a GIGA R1 WiFi (ABX00063) board.
 * 2. Upload this sketch to your board.
 * 3. Open the Serial Monitor.
 * 4. Touch the screen with your finger(s) and change the LED status on the GIGA R1 WiFi.
 *      | Screen section | LED   |
 *      |----------------|-------|
 *      | Left third     | Red   |
 *      | Middle third   | Green |
 *      | Right third    | Blue  |
 * 
 * Initial author: Ali Jahangiri @aliphys
 * Created: 12 June 2024
 */

#include "Arduino_GigaDisplayTouch.h"

Arduino_GigaDisplayTouch touchDetector;

bool LEDRed = false;
bool LEDGreen = false;
bool LEDBlue = false;

unsigned long last_touch_time = 0;

void setup() {
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {}

  if (touchDetector.begin()) {
    Serial.println("Touch controller init - OK");
  } else {
    Serial.println("Touch controller init - FAILED");
    while (1);
  }
}


void loop() {
  uint8_t contacts;
  GDTpoint_t points[5];

  contacts = touchDetector.getTouchPoints(points);

  unsigned long touch_time = millis();
  if (contacts > 0 && touch_time - last_touch_time > 300) {
    // right third of the screen
    if (points[0].y > 520) {
      LEDBlue = !LEDBlue;
      digitalWrite(LEDB, LEDBlue);
      Serial.print("Blue LED State Changed: ");
      Serial.println(!LEDBlue);
    }
    // middle third of the screen
    else if (points[0].y > 260) {
      LEDGreen = !LEDGreen;
      digitalWrite(LEDG, LEDGreen);
      Serial.print("Green LED State Changed: ");
      Serial.println(!LEDGreen);
    }
    // left third of the screen
    else {
      LEDRed = !LEDRed;
      digitalWrite(LEDR, LEDRed);
      Serial.print("Red LED State Changed: ");
      Serial.println(!LEDRed);
    }

    last_touch_time = touch_time;
  }

  delay(1);
}