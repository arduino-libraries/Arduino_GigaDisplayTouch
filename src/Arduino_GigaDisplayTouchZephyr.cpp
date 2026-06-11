/*
 * Copyright 2025 Arduino SA
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifdef __ZEPHYR__

#include "Arduino_GigaDisplayTouch.h"
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#if __has_include("lvgl.h")
#include "lvgl.h"
static Arduino_GigaDisplayTouch *gThis;
void _lvglTouchCb(lv_indev_t *indev, lv_indev_data_t *data);

#ifndef LVGL_VERSION_MAJOR
#error "LVGL_VERSION_MAJOR is not defined. Please include a valid LVGL v9.x.x installation."
#endif

#if LVGL_VERSION_MAJOR < 9
#error "Arduino_GigaDisplayTouch library supports only LVGL version 9.x.x or higher."
#endif
#endif

static struct k_sem touch_sem;

typedef void (*zephyr_input_callback_t)(struct input_event *evt,
                                        void *user_data);
extern "C" void zephyr_input_register_callback(zephyr_input_callback_t cb,
                                               void *user_data);
void touch_event_callback(struct input_event *evt, void *user_data);

Arduino_GigaDisplayTouch::Arduino_GigaDisplayTouch()
  : Arduino_GigaDisplayTouch(Wire1) {}

Arduino_GigaDisplayTouch::Arduino_GigaDisplayTouch(TwoWire &wire)
  : _wire{ wire } {}

Arduino_GigaDisplayTouch::~Arduino_GigaDisplayTouch() {}

bool Arduino_GigaDisplayTouch::begin() {
  static const struct device *const dev =
    DEVICE_DT_GET(DT_CHOSEN(zephyr_touch));
  if (!dev) {
    printk("<ERR> touch DEV null\n");
    return false;
  }

  _wire.begin();

  if (!device_is_ready(dev)) {
    // init device for first usage, if not ready
    int err = device_init(dev);
    if (err < 0) {
      return false;
    }
  }

  _gt911TouchHandler = nullptr;

#if __has_include("lvgl.h")
  gThis = this;
#endif
  // Initialize to 1 to prevent deadlock by ensuring that
  // at least one function can always proceed initially.
  k_sem_init(&touch_sem, 1, 1);
  zephyr_input_register_callback(touch_event_callback, this);

#if __has_include("lvgl.h")
  static lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, _lvglTouchCb);
#endif

  return true;
}

#if __has_include("lvgl.h")
#if (LVGL_VERSION_MAJOR == 9)
void _lvglTouchCb(lv_indev_t *indev, lv_indev_data_t *data) {
#else
void _lvglTouchCb(lv_indev_drv_t *indev, lv_indev_data_t *data) {
#endif
  GDTpoint_t points[GT911_MAX_CONTACTS];

  // Usiamo getTouchPoints che gestisce già il semaforo in modo non bloccante
  uint8_t contacts = gThis->getTouchPoints(points);

  if (contacts > 0) {
    // LVGL standard gestisce un punto alla volta (il primo dito)
#if (LVGL_VERSION_MAJOR == 9)
    data->state = LV_INDEV_STATE_PRESSED;
#else
    data->state = LV_INDEV_STATE_PR;
#endif
    data->point.x = points[0].x;
    data->point.y = points[0].y;
  } else {
#if (LVGL_VERSION_MAJOR == 9)
    data->state = LV_INDEV_STATE_RELEASED;
#else
    data->state = LV_INDEV_STATE_REL;
#endif
  }
}
#endif

void Arduino_GigaDisplayTouch::end() {
  _gt911TouchHandler = nullptr;
  zephyr_input_register_callback(NULL, NULL);
  memset(_points, 0, sizeof(_points));
}

uint8_t Arduino_GigaDisplayTouch::getTouchPoints(GDTpoint_t *points) {
  // First wait to see if we get any events.
  if (k_sem_take(&touch_sem, K_NO_WAIT)) {
    return 0;
  }

  size_t count_pressed = 0;
  for (int i = 0; i < GT911_MAX_CONTACTS; i++) {
    if (_points[i].pressed) {
      _points[i].pressed = 0;
      points[count_pressed].trackId = _points[i].trackId;
      points[count_pressed].x = _points[i].x;
      points[count_pressed].y = _points[i].y;
      count_pressed++;
    }
  }

  k_sem_give(&touch_sem);
  return count_pressed;
}

void Arduino_GigaDisplayTouch::onDetect(GDTTouchHandler_t handler) {
  _gt911TouchHandler = handler;
}

void touch_event_callback(struct input_event *evt, void *user_data) {
  static int8_t index = 0;
  static bool sem_taken = false;

  static const struct device *const dev =
    DEVICE_DT_GET(DT_CHOSEN(zephyr_touch));
  Arduino_GigaDisplayTouch *touch = (Arduino_GigaDisplayTouch *)user_data;

  if (!touch || evt->dev != dev) {
    return;
  }

  // Take semaphore on first event.
  if (evt->code == INPUT_ABS_MT_SLOT) {
    // Check if the semaphore is already taken by this callback.
    // This could only happen if the event queue dropped BTN_TOUCH.
    if (!sem_taken && k_sem_take(&touch_sem, K_NO_WAIT) != 0) {
      return;
    }
    sem_taken = true;
  } else if (!sem_taken) {
    // On subsequent events, return if we don't have the semaphore.
    return;
  }

  switch (evt->code) {
    case INPUT_ABS_MT_SLOT:
      index = evt->value;
      touch->_points[index].trackId = evt->value;
      break;
    case INPUT_ABS_X:
      touch->_points[index].x = evt->value;
      break;
    case INPUT_ABS_Y:
      touch->_points[index].y = evt->value;
      break;
    case INPUT_BTN_TOUCH:
      touch->_points[index].pressed = evt->value;
      break;
  }

  // Release the semaphore on the last event (BTN_TOUCH pressed).
  if (evt->code == INPUT_BTN_TOUCH && evt->value) {
    sem_taken = false;
    k_sem_give(&touch_sem);
    if (touch->_gt911TouchHandler) {
      uint8_t count_pressed = 0;
      for (int i = 0; i < GT911_MAX_CONTACTS; i++) {
        if (touch->_points[i].pressed) {
          touch->_callback_points[count_pressed].trackId =
            touch->_points[i].trackId;
          touch->_callback_points[count_pressed].x = touch->_points[i].x;
          touch->_callback_points[count_pressed].y = touch->_points[i].y;
          count_pressed++;
        }
      }
      touch->_gt911TouchHandler(count_pressed, touch->_callback_points);
    }
  }
}
#endif
