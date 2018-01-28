### Description

ESP8266 / Arduino code for matrixpainter.net

### Features

- Receive data over MQTT
- Sent to grid of Neopixels
- Setup wifi details using softAp

### Instructions

Install:
- https://github.com/esp8266/Arduino
- https://github.com/tzapu/WiFiManager
- https://github.com/adafruit/Adafruit_NeoPixel
- https://github.com/adafruit/Adafruit_MQTT_Library

Rename connection_details_example.h to connection_details.h and fill in the blanks

Flash the ESP8266

Power up and pull down pin 0 to ground (the flash button on the MatrixPainter board)

Join the "MatrixPainter" wifi network, and enter your wifi details.

Done!

### License

This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.

<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />
Designed by <a xmlns:cc="http://creativecommons.org/ns#" href="http://www.richardhawthorn.com" property="cc:attributionName" rel="cc:attributionURL" target="_blank">Richard Hawthorn</a><br />
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/" target="_blank">View License</a>