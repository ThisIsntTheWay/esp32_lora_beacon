#include <TTN_esp32.h>
#include "TTN_CayenneLPP.h"
#include "heltec.h"

// Set LoRa Band (868MHz, Europe)
#define BAND    868E6 

// TTN stuff
const char* devEui = "- - -"; // Change to TTN Device EUI
const char* appEui = "- - -"; // Change to TTN Application EUI
const char* appKey = "- - -"; // Chaneg to TTN Application Key

unsigned int counter = 0;

// Init libraries
TTN_esp32 ttn ;
TTN_CayenneLPP lpp;

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

    // TTN JOIN ROUTINE
    ttn.begin();
    Heltec.display->drawString(0, 20, " > Joining TTN...");
    Heltec.display->display();
    ttn.onMessage(message); // Declare callback function for handling downlink
                            // messages from server
    ttn.join(devEui, appEui, appKey);
    Serial.print("Joining TTN ");
    while (!ttn.isJoined())
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\njoined !");
    Heltec.display->drawString(0, 30, " > TTN joined.");
    Heltec.display->display();
    ttn.showStatus();
    Heltec.display->drawString(0, 40, "Init concluded!");
    Heltec.display->display();

    delay(1000);
}

void loop()
{
    static float nb = 18.2;
    nb += 0.1;

    // Construct lpp
    lpp.reset();
    lpp.addTemperature(1, nb);

    if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize())) {
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
