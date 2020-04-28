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
The purpose of this project is to build a LoRaWAN coverage mapper, to be used in conjunction with "TTN Mapper phone surveyor".
As it spits out GPS sensor data every few seconds, this can also work as a GPS tracker.

### Hardware
Technically, any ESP32 based dev board will work with this project.<br/>
As long as access to UART1/UART2 is guaranteed, the board can be used.

If an external LoRa module is used, make sure that it is compatible with the TTN_32 library!

Any GPS module will work, as long as it's able to spit out NMEA messages directly.<br/>
(I.e. doesn't produce data that can't be parsed by TinyGPS++.)

### ToDo
- Implement GPS
  - Currently on hold because my GPS sensor is DOA.
