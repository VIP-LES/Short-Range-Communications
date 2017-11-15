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

// Include the FRM69 and SPI libraries

#include <SPI.h>
#include <RFM69.h>

// Define radio info

#define NETWORKID     0   // Must be the same for all nodes (0 to 255)
#define MYNODEID      2   // My node ID (0 to 255)
#define TONODEID      1   // Destination node ID (0 to 254, 255 = broadcast)
#define FREQUENCY     RF69_915MHZ
#define USEACK        true

#define SIGNATURE     (uint8_t) 0b101011   // Signature to check for confirmation
#define FLAG_ALARM    (uint8_t) 0  // The bit for the alarm toggle
#define FLAG_CUTDOWN  (uint8_t) 1  // The bit for the cutdown toggle

#define CUTDOWN_PIN   7
#define ALARM_PIN     6

#define WAIT_DELAY    750 // If a response is not received within 750ms of a response, we cut off

RFM69 radio;

void setup() {
    pinMode(ALARM_PIN, OUTPUT);
    digitalWrite(ALARM_PIN, LOW);

    pinMode(CUTDOWN_PIN, OUTPUT);
    digitalWrite(CUTDOWN_PIN, LOW);

    Serial.begin(9600);
    Serial.println("Starting cutdown system");

    radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
    radio.setHighPower(); // Always use this for RFM69HCW
}

unsigned long last_receipt = millis();
unsigned long current_time = millis();

int cutdown = 0;
int alarm = 0;
uint8_t packet = 0;

void loop() {
    current_time = millis();
    if (radio.receiveDone()) {
        if (radio.DATALEN == 1) {
            packet = (uint8_t) radio.DATA[0];

            if (packet >> 2 == SIGNATURE) {
                Serial.println("Received packet.");

                last_receipt = current_time;

                // Acknowledge it
                if (radio.ACKRequested())
                    radio.sendACK();

                Serial.println(radio.DATA[0]);

                if (packet & (1 << FLAG_ALARM) != 0)
                    alarm = 1;
                else
                    alarm = 0;

                if (packet & (1 << FLAG_CUTDOWN) != 0)
                    cutdown = 1;
                else
                    cutdown = 0;
            }
        }
    }

    if (current_time - last_receipt > WAIT_DELAY) {
        alarm = 0;
        cutdown = 0;
    }

    digitalWrite(ALARM_PIN, alarm ? HIGH : LOW);
    digitalWrite(CUTDOWN_PIN, cutdown ? HIGH : LOW);
}
