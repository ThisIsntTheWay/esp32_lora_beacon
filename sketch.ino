// Libraries
#include <TTN_esp32.h>
#include "TTN_CayenneLPP.h"
#include "heltec.h"
#include "TinyGPS++.h"

// Set LoRa Band (868MHz, Europe)
#define BAND    868E6

// TTN stuff
const char* devEui = "- - -"; // Change to TTN Device EUI
const char* appEui = "- - -"; // Change to TTN Application EUI
const char* appKey = "- - -"; // Chaneg to TTN Application Key

unsigned int counter = 0;

// Init libraries
TTN_esp32 ttn;
TTN_CayenneLPP lpp;
TinyGPS++ gps;

// TTN "Join" message
// Mainly used for telemetry.
void message(const uint8_t* payload, size_t size, int rssi)
{
    Serial.println("-- MESSAGE");
    Serial.print("Received " + String(size) + " bytes RSSI=" + String(rssi) + "db");
    for (int i = 0; i < size; i++)
    {
        Serial.print(" " + String(payload[i]));
    }

    Serial.println();
}

void setup()
{
    // Init Heltec OLED
    Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
    Heltec.display->init();
    Heltec.display->flipScreenVertically();
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->clear();
    
    Heltec.display->drawString(0, 0, "TTN Beacon - v0.5");
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
        // Keep printing '...' as long as TTN has not yet been joined.
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
    // Construct lpp buffer
    lpp.reset();
    lpp.addGPS(1, gLat, gLon, gAlt);

    // Send out lpp packet to TTN
    if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize())) {
        // Write sent lpp packet to serial.
        Serial.printf("Temp: %f TTN_CayenneLPP: %d %x %02X%02X\n", nb, lpp.getBuffer()[0], lpp.getBuffer()[1],
            lpp.getBuffer()[2], lpp.getBuffer()[3]);
    }

    // Write to OLED
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    
    Heltec.display->drawString(0, 0, "Packets sent: ");
    Heltec.display->drawString(90, 0, String(counter));
    Heltec.display->display();
    counter++;
    delay(3000);
}
