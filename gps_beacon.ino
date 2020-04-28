// Libraries
#include <TTN_esp32.h>
#include "TTN_CayenneLPP.h"
#include "heltec.h"
#include "TinyGPS++.h"
#include <HardwareSerial.h>

// Set LoRa Band (868MHz, Europe)
#define BAND    868E6

// TTN stuff
// NOTE: The values requested are only applicable to a TTN-join via OTA!
const char* devEui = "- - -"; //TTN Device EUI
const char* appEui = "- - -"; //TTN Application EUI
const char* appKey = "- - -"; //TTN Application Key

// HW Serial and GPS config
// NOTE: Here, it's set to UART2.
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;

unsigned int counter = 0;

// Init libraries
TTN_esp32 ttn;
TTN_CayenneLPP lpp;
TinyGPSplus gps;
HardwareSerial sGPS(1);

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
    sGPS.begin(GPSBaud, SERIAL_8IN1, RXPin, TXPin);
    
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
    // Encode/parse NMEA messages if the HW serial is available.
    while (sGPS.available() > 0) {
        gps.encode(sGPS.read());
    }
    
    // Construct lpp buffer.
    lpp.reset();
    lpp.addGPS(1, gps.location.lat(), gps.location.lng(), gps.altitude.meters());

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
    Heltec.display->drawString(0, 10, "GPS Satellites: ");
    Heltec.display->drawString(90, 10, String(gps.satellites.value()));    
    Heltec.display->drawString(0, 20, "Perceived HDOP: ");
    Heltec.display->drawString(90, 20, String(gps.hdop.value()));
    Heltec.display->display();
    counter++;
    delay(3000);
}
