// Modified for controlling an alarm and cutdown circuit
// on a high altitude balloon payload by Matthew Pruett

// Based on RFM69 library sample code by Felix Rusu
// http://LowPowerLab.com/contact
// Modified for RFM69HCW by Mike Grusin, 4/16

// This sketch will show you the basics of using an
// RFM69HCW radio module. SparkFun's part numbers are:
// 915MHz: https://www.sparkfun.com/products/12775
// 434MHz: https://www.sparkfun.com/products/12823

// See the hook-up guide for wiring instructions:
// https://learn.sparkfun.com/tutorials/rfm69hcw-hookup-guide

// Uses the RFM69 library by Felix Rusu, LowPowerLab.com
// Original library: https://www.github.com/lowpowerlab/rfm69
// SparkFun repository: https://github.com/sparkfun/RFM69HCW_Breakout

// Include the RFM69 and SPI libraries:

#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      2   // Destination node ID (0 to 254, 255 = broadcast)
#define FREQUENCY     RF69_915MHZ
#define USEACK        true

#define SIGNATURE     (uint8_t) 0b101011   // Signature to check for confirmation
#define FLAG_ALARM    (uint8_t) 0  // The bit for the alarm toggle
#define FLAG_CUTDOWN  (uint8_t) 1  // The bit for the cutdown toggle

// How long should we delay between two messages?
#define SEND_DELAY    500

#define CUTDOWN       6
#define ALARM         5
#define LED           8

// Create a library object for our RFM69HCW module:
RFM69 radio;

void setup() {
    Serial.begin(9600);
    Serial.print("Node ");
    Serial.print(MYNODEID,DEC);
    Serial.println(" ready");

    // Set up the toggle switches and LED:
    pinMode(CUTDOWN, INPUT_PULLUP);
    pinMode(ALARM, INPUT_PULLUP);
    pinMode(LED, OUTPUT);

    // Initialize the RFM69HCW:
    radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
    radio.setHighPower(); // Always use this for RFM69HCW

}

uint8_t packet = 0;
void loop() {
    // SENDING
    // In this section, we'll gather toggle switch inputs
    // and send a single byte containing our signature and
    // the states of the two toggles

    packet = SIGNATURE << 2;

    if (!digitalRead(CUTDOWN))
        packet |= 1 << FLAG_CUTDOWN;

    if (!digitalRead(ALARM))
        packet |= 1 << FLAG_ALARM;

    // There are two ways to send packets. If you want
    // acknowledgements, use sendWithRetry():
    if (USEACK)
    {
        if (radio.sendWithRetry(TONODEID, &packet, 1, 0)) {
            digitalWrite(LED, HIGH); // Leave it on until the next packet
        } else {
            digitalWrite(LED, LOW);
        }
    } else {
        radio.send(TONODEID, &packet, 1);
    }

    delay(SEND_DELAY);
}
