# Arduino Giga Display Touch Library

[![License](https://img.shields.io/badge/License-LGPLv3-blue.svg)]() [![Check Clang status](https://github.com/arduino-libraries/Arduino_GigaDisplayTouch/actions/workflows/check-clang.yml/badge.svg)](https://github.com/arduino-libraries/Arduino_GigaDisplayTouch/actions/workflows/check-clang.yml)

The Arduino Giga Display Touch library is a C++ library designed to manage the touch controller of the Giga Display Shield. It is currently compatible with Goodix GT911 5-point Capacitive touch controller. 

This library captures up to 5 concurrent touch points.

The library provides methods to initialize the touch controller, detect touch points, and access to the coordinates of the points. It supports two configuration options: by interrupt or on-request. With the interrupt approach, the library notifies the user when a touch is detected, while with on-request, it is the user's responsibility to query the library to determine if a touch has occurred.

## Features

- Captures up to 5 concurrent touch points
- Retrieve touch points coordinates
- Supports dual configuration: interrupt or on-request

## Usage

To use this library, you must have a supported Arduino board and the Arduino Giga Display shield. Once you have connected the display shield to the board, you can include the touch library in your Arduino sketch and use its functions to capture touch points and access to the coordinate values. Here is a minimal example for the Arduino GIGA R1 WiFi:

```cpp
#include "Arduino_GigaDisplayTouch.h"

Arduino_GigaDisplayTouch touchDetector;

void setup() {
    Serial.begin(115200);
    touchDetector.begin();
}

void loop() {
    uint8_t contacts;
    GDTpoint_t points[5];
    
    contacts = touchDetector.getTouchPoints(points);
    if (contacts > 0) { //Check if at least one touch occurs on the screen
        //Print the coordinates of all simultaneous contacts detected
        for (uint8_t i = 0; i < contacts; i++) {
            Serial.print(points[i].x);
            Serial.print(" ");
            Serial.println(points[i].y);
        }
        //Do something with the touch coordinates
    }
}
```
## Examples

- **[Touch_IRQ](../examples/Touch_IRQ):** This example demonstrates how to detect touch event and retrieve coordinate values using the interrupt approach.
- **[Touch_Polling](../examples/Touch_Polling):** This example demonstrates how to detect touch event and retrieve coordinate values using the polling/on-request approach.

## API

The API documentation can be found [here](./api.md).

## License

This library is released under the [LGPLv3 license]().