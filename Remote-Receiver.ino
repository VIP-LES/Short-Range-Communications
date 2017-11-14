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
#define ALARM         6

int soundAlarm = 0;
int activateCutdown = 0;
unsigned long lastPacket = 0;

RFM69 radio;


//pin 7 sends cutdown signal to balloon
int trigger_out = 7;
int cutdown_trys = 0;

void setup() {

  pinMode(ALARM, OUTPUT);
  
  
  pinMode(trigger_out, OUTPUT);
  digitalWrite(trigger_out, LOW);
  Serial.begin(9600);
  Serial.println("Balloon cutdown!");

  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW
  
  
}



uint32_t timer = millis();
uint32_t starttimer = millis();

void loop() {   
  
  if (radio.receiveDone()) // Got one!
  {
   
    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:
    Serial.println("Received packet");
    Serial.println(radio.DATA[0]);
    Serial.println(radio.DATA[1]);

    if (radio.DATALEN == 2) {
      activateCutdown = (int)radio.DATA[0];
      soundAlarm = (int)radio.DATA[1];
      lastPacket = millis();
    }

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)
    
    if (radio.ACKRequested())
    {
      radio.sendACK();
    }
  }

  if (lastPacket < millis() - 2000) {
    activateCutdown = 0;
    soundAlarm = 0;
  }

  if (soundAlarm) {
    digitalWrite(ALARM, HIGH);
  } else {
    digitalWrite(ALARM, LOW);
  }

  //if((millis() - starttimer > 15000) || activateCutdown){
  if (activateCutdown) {
    digitalWrite(trigger_out, HIGH);
    delay(5000);
    Serial.println("Cutdown");
    Serial.println("Attempt");
    Serial.println(cutdown_trys);
    cutdown_trys = cutdown_trys + 1;
    digitalWrite(trigger_out, LOW);
  }
}
      
      

