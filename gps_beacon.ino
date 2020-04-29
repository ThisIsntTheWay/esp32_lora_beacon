/*
*   This is a GPS tracker based on an ESP32 that is LoRa-capable.   
*   Author: vk, contact@klopfen.rocks
*
*   Version: 1.1
*   (Not yet tested)
*/

// Libraries
#include <TTN_esp32.h>
#include "TTN_CayenneLPP.h"
#include "heltec.h"
#include "TinyGPS++.h"
#include <HardwareSerial.h>

// Set LoRa Band (868MHz, Europe)
#define BAND 868E6

// TTN stuff
// NOTE: The values requested are only applicable to a TTN-join via OTA!
const char* devEui = "- - -"; //TTN Device EUI
const char* appEui = "- - -"; //TTN Application EUI
const char* appKey = "- - -"; //TTN Application Key

// HW Serial and GPS config
// NOTE: Here, it's set to UART2.
#define RXD2    23
#define TXD2    17
#define GPSBaud 9600

// MISC things
#define satThreshold 4;
unsigned int counter = 0;
bool realiableFix = false;

// Init libraries
TTN_esp32 ttn;
TTN_CayenneLPP lpp;
TinyGPSPlus gps;

// TTN "Join" message
void message(const uint8_t* payload, size_t size, int rssi)
{
    Serial.println("-- MESSAGE");
    Serial.print("Received " + String(size) + " bytes RSSI=" + String(rssi) + "db");
    for (int i = 0; i < size; i++) {
        Serial.print(" " + String(payload[i]));
    }

    Serial.println();
}

void setup()
{
    // Start listening to HW Serial channel
    Serial2.begin(GPSBaud, SERIAL_8N1, RXD2, TXD2);
    
    // Init Heltec OLED
    Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
    Heltec.display->init();
    Heltec.display->flipScreenVertically();
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->clear();
    
    Heltec.display->drawString(0, 0, "TTN Beacon - v1.0");
    Heltec.display->drawString(0, 10, "Initialising...");
    Heltec.display->display();
  
    Serial.begin(115200);
    Serial.println("Starting");

    // Join TTN
    ttn.begin();
    Heltec.display->drawString(0, 20, " > Joining TTN...");
    Heltec.display->display();
    ttn.onMessage(message); // Declare callback function for handling downlink
                            // messages from server.
    
    ttn.join(devEui, appEui, appKey);
    Serial.print("Joining TTN ");
    while (!ttn.isJoined())
    {
        // Keep printing '...' as long as TTN hasn't been joined yet.
        Serial.print(".");
        delay(500);
    }
    Serial.println("\njoined !");
    Heltec.display->drawString(0, 30, " > TTN joined.");
    Heltec.display->display();
    ttn.showStatus(); // <-- Only visible on a serial monitor.
    
    Heltec.display->drawString(0, 40, "Init concluded!");
    Heltec.display->display();

    delay(1000);
}

void loop()
{           
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    
    // Encode/parse NMEA messages if the HW serial is available.
    while (Serial2.available() > 0) {
        gps.encode(Serial2.read());
    }

    // Convert TinyGPS++ output to float.
    // CayenneLPP cannot interpret the output otherwise.
    float gLat = gps.location.lat();
    float gLng = gps.location.lng();
    float gAlt = gps.altitude.meters();
    int gSat = gps.satellites.value();
    
    // Determine quality of GPS lock
    if (gSat < satThreshold) {
        reliableFix = false;
    } else {
        reliableFix = true;
    }
    
    if (reliableFix) {    
        // Construct lpp buffer.
        lpp.reset();
        lpp.addGPS(1, gLat, gLng, gAlt);

        // Send out lpp packet to TTN
        if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize())) {
            // Write sent lpp packet to serial.
            Serial.printf("cLPP packet: %f %d %02X%02X\n", lpp.getBuffer()[0], lpp.getBuffer()[1],
                lpp.getBuffer()[2], lpp.getBuffer()[3]);
        } else {
            Serial.println("cLPP packet transmission failure.");
            Heltec.display->drawString(0, 40, "cLPP packet failure!");
        }

        // Spit out info to serial
        Serial.println();
        Serial.print("LAT - "); Serial.println(gps.location.lat());
        Serial.print("LON - "); Serial.println(gps.location.lng());
        Serial.print("ALT - "); Serial.println(gps.altitude.meters());
        Serial.print("SAT - "); Serial.println(gps.satellites.value());
        Serial.print("HDOP - "); Serial.println(gps.hdop.value());
        Serial.println("-----------------------");

        // Write to OLED
        Heltec.display->drawString(0, 0, "Packets sent: ");
        Heltec.display->drawString(90, 0, String(counter));    
        Heltec.display->drawString(0, 20, "GPS Satellites: ");
        Heltec.display->drawString(90, 20, String(gSat));    
        Heltec.display->drawString(0, 30, "Perceived HDOP: ");
        Heltec.display->drawString(90, 30, String(gps.hdop.value()));
        Heltec.display->display();
    } else {
        Serial.println("No reliable GPS fix.");
        Serial.printf("Detected satellites: %f", String(gSat)); Serial.println();   
        
        Heltec.display->drawString(0, 0, "[x] No reliable GPS fix."); 
        Heltec.display->drawString(0, 20, "Satellites:");
        Heltec.display->drawString(90, 20, String(gSat));
        Heltec.display->drawString(0, 30, "Expected:");
        Heltec.display->drawString(90, 30, String(satThreshold));
    }
    
    counter++;
    delay(3000);
}
