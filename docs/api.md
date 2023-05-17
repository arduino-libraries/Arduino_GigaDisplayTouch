# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class` [`Arduino_GigaDisplayTouch`](#) | The main class for managing the touch controller of the Giga Display Shield. 

# class `Arduino_GigaDisplayTouch` 
The main class for managing the touch controller of the Giga Display Shield. 

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public ` [`Arduino_GigaDisplayTouch`](#)`(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr)` | Construct a new touch controller for Giga Display Shield.
`public bool` [`begin`](#)`()` | Initialize the touch controller.
`public void` [`end`](#)`()` | De-initialize the touch controller.
`public uint8_t` [`getTouchPoints`](#)`(GDTpoint_t* points)` | Check if a touch event is detected and get the touch points.
`public void` [`onDetect`](#)`(void (*handler)(uint8_t, GDTpoint_t*))` | Attach an interrupt handler function for touch detection callbacks.

## Members

### `public ` [`Arduino_GigaDisplayTouch`](#)`(TwoWire& wire, uint8_t intPin, uint8_t rstPin, uint8_t addr)`

Construct a new touch controller for Giga Display Shield.

#### Parameters
* `wire` A reference to the Wire interface to be used for communication with the touch controller.

* `intPin` The interrupt pin number for the touch controller.

* `rstPin` The reset pin number for the touch controller.

* `addr` The device address for the touch controller.

### `public bool` [`begin`](#)`()` 

Initialize the touch controller.

#### Returns
true If the touch controller is successfully initialized, false otherwise

### `public void` [`end`](#)`()` 

De-initialize the touch controller.

### `public uint8_t` [`getTouchPoints`](#)`(GDTpoint_t* points)` 

Check if a touch event is detected and get the touch points.

#### Parameters
* `points` The array containing the coordinates of the touch points.

#### Returns
uint8_t The number of detected touch points.

### `public void` [`onDetect`](#)`(void (*handler)(uint8_t, GDTpoint_t*))` 

Attach an interrupt handler function for touch detection callbacks.

#### Parameters
* `handler` The pointer to the user-defined handler function.