# esp32_lora_beacon
LoRa beacon based on a WiFi_LoRa_32_v2 board.

Utilizes the following libraries:
- TTN_32: https://github.com/rgot-org/TheThingsNetwork_esp32
  - Implements an interface with TTN and the LoRa module.
  - Incorporates CayenneLPP as well!
- Heltec_ESP32: https://github.com/HelTecAutomation/Heltec_ESP32
  - Mainly for interfacing with the onboard OLED screen.
- TinyGPS++: http://arduiniana.org/libraries/tinygpsplus/
  - For parsing NMEA messages.

### Intent
The purpose of this project is to build a GPS tracker that communicates with 'The Things Network', based on LoRa tech.<br/>
Can also be used as a TTN coverage mapper.<br/>
###### (If used as such, be aware that the produced data will have less priority as it is in the CayenneLPP format!)

### Hardware
Technically, any ESP32 based dev board will work with this project:<br/>
As long as access to UART1/UART2 is guaranteed, the board can be used.

If an external LoRa module is used, make sure that it is compatible with the TTN_32 library!

Any GPS module will work, as long as it's able to spit out NMEA messages directly.<br/>
(I.e. doesn't produce data that can't be parsed by TinyGPS++.)

### Notes
The UART2 RX pin is set to 23 instead of the standard 16, because on heltec this pin is used by OLED_RST already.</br>
This has distorted the GPS output to the point of it becoming unusable. (YMMV)
