/*
  LVGL_Touch

  created 05 May 2023
  by Leonardo Cavagnis
*/

#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"

#include "lvgl.h"
#include "../examples/lv_examples.h"

Arduino_H7_Video            Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch    Touch;

void setup() {
  Display.begin();
  Touch.begin();
  
  /* Example: Button click event - https://docs.lvgl.io/master/examples.html#events */
  lv_example_event_1();
}

void loop() { 
  /* Feed LVGL engine */
  lv_timer_handler();
}
