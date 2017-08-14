/*
 Example sketch for the PS3 Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */

// PS3
#include <PS3BT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

//Lights
#include "RGBdriver.h"
#define CLK 2//pins definitions for the driver        
#define DIO 3
RGBdriver Driver(CLK,DIO);


int DELAY;
int LOOPS;
bool PAUSE;
bool lightToggle;
bool strobeToggle; // Wheather to strobe
bool strobeValue;  // If off or on


USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
PS3BT PS3(&Btd); // This will just create the instance
//PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

bool printTemperature;
bool printAngle;

void setup() {
  Serial.begin(115200);


  DELAY = 30; //Milli seconds
  LOOPS = 40; //How many time the lights flash
  PAUSE = false; //Stops the program from doing anything
  lightToggle = false; // Set light to default false
  strobeToggle = false; // Set strobe to false
  strobeValue = false;  // Set strobe value low, true sets it high

  Driver.begin(); // begin
  Driver.SetColor(0, 0, 0); // Set to No Color
  Driver.SetColor(0, 0, 0); // Set to No Color
  Driver.end();

  
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));
}
void loop() {
  Usb.Task();

  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    Serial.print("Connected");
    
    if (PS3.getAnalogHat(LeftHatX) > 137 || PS3.getAnalogHat(LeftHatX) < 117 || PS3.getAnalogHat(LeftHatY) > 137 || PS3.getAnalogHat(LeftHatY) < 117 || PS3.getAnalogHat(RightHatX) > 137 || PS3.getAnalogHat(RightHatX) < 117 || PS3.getAnalogHat(RightHatY) > 137 || PS3.getAnalogHat(RightHatY) < 117) {
      Serial.print(F("\r\nLeftHatX: "));
      Serial.print(PS3.getAnalogHat(LeftHatX));
      Serial.print(F("\tLeftHatY: "));
      Serial.print(PS3.getAnalogHat(LeftHatY));
      if (PS3.PS3Connected) { // The Navigation controller only have one joystick
        Serial.print(F("\tRightHatX: "));
        Serial.print(PS3.getAnalogHat(RightHatX));
        Serial.print(F("\tRightHatY: "));
        Serial.print(PS3.getAnalogHat(RightHatY));
      }
    }

    // Analog button values can be read from almost all buttons
    if (PS3.getAnalogButton(L2) || PS3.getAnalogButton(R2)) {
      Serial.print(F("\r\nL2: "));
      Serial.print(PS3.getAnalogButton(L2));
      if (PS3.PS3Connected) {
        Serial.print(F("\tR2: "));
        Serial.print(PS3.getAnalogButton(R2));
      }
    }
    if (PS3.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS3.disconnect();
    }
    else {
      if (PS3.getButtonClick(TRIANGLE))
        Serial.print(F("\r\nTraingle"));
      if (PS3.getButtonClick(CIRCLE))
        Serial.print(F("\r\nCircle"));
      if (PS3.getButtonClick(CROSS))
        Serial.print(F("\r\nCross"));
      if (PS3.getButtonClick(SQUARE))
        Serial.print(F("\r\nSquare"));

      if (PS3.getButtonClick(UP)) {
        Serial.print(F("\r\nUp"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED4);
        }
      }
      if (PS3.getButtonClick(RIGHT)) {
        Serial.print(F("\r\nRight"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED1);
        }
      }
      if (PS3.getButtonClick(DOWN)) {
        Serial.print(F("\r\nDown"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED2);
        }
      }
      if (PS3.getButtonClick(LEFT)) {
        Serial.print(F("\r\nLeft"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED3);
        }
      }

      if (PS3.getButtonClick(L1))
        Serial.print(F("\r\nL1"));
      if (PS3.getButtonClick(L3))
        Serial.print(F("\r\nL3"));
      if (PS3.getButtonClick(R1))
        Serial.print(F("\r\nR1"));
      if (PS3.getButtonClick(R3))
        Serial.print(F("\r\nR3"));

      if (PS3.getButtonClick(SELECT)) {
        Serial.print(F("\r\nSelect - "));
        PS3.printStatusString();
      }
      if (PS3.getButtonClick(START)) {
        Serial.print(F("\r\nStart"));
        printAngle = !printAngle;
      }
    }
#if 0 // Set this to 1 in order to see the angle of the controller
    if (printAngle) {
      Serial.print(F("\r\nPitch: "));
      Serial.print(PS3.getAngle(Pitch));
      Serial.print(F("\tRoll: "));
      Serial.print(PS3.getAngle(Roll));
    }
#endif
  }
#if 1 // Set this to 1 in order to enable support for the Playstation Move controller
  else if (PS3.PS3MoveConnected) {
    if (PS3.getAnalogButton(T)) {
      Serial.print(F("\r\nT: "));
      Serial.print(PS3.getAnalogButton(T));
    }
    if (PS3.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS3.disconnect();
    }
    else {
      if (PS3.getButtonClick(SELECT)) {
        Serial.print(F("\r\nSelect"));
        printTemperature = !printTemperature;
      }
      if (PS3.getButtonClick(START)) {
        Serial.print(F("\r\nStart"));
        printAngle = !printAngle;
      }
      if (PS3.getButtonClick(TRIANGLE)) {
        purple();
        Serial.print(F("\r\nTriangle"));
        PS3.moveSetBulb(Red);
      }
      if (PS3.getButtonClick(CIRCLE)) {
        strobe();
        lightsOff();
        Serial.print(F("\r\nCircle"));
        PS3.moveSetBulb(Green);
      }
      if (PS3.getButtonClick(SQUARE)) {
        orange();
        Serial.print(F("\r\nSquare"));
        PS3.moveSetBulb(Blue);
      }
      if (PS3.getButtonClick(CROSS)) {
        Serial.print(F("\r\nCross"));
        PS3.moveSetBulb(Yellow);
      }
      if (PS3.getButtonClick(MOVE)) {

        toggleLights();
        
        PS3.moveSetBulb(Off);
        Serial.print(F("\r\nMove"));
        Serial.print(F(" - "));
        PS3.printStatusString();
      }
    }
    if (printAngle) {
      Serial.print(F("\r\nPitch: "));
      Serial.print(PS3.getAngle(Pitch));
      Serial.print(F("\tRoll: "));
      Serial.print(PS3.getAngle(Roll));
    }
    else if (printTemperature) {
      Serial.print(F("\r\nTemperature: "));
      Serial.print(PS3.getTemperature());
    }
  }
#endif
  strobeCheck();
}


void toggleLights() {
  if (lightToggle) {
    Serial.print("\nLights Off");
    Driver.begin(); // begin
    Driver.SetColor(100, 0, 0); // Set to RED Color
    Driver.end();
    lightToggle = false;
  } else {
    Serial.print("\nLights On");
    Driver.begin(); // begin
    Serial.print("\nBefore Color change");
    Driver.SetColor(0, 0, 100); // Set to Blue Color
    Serial.print("\nAfter Color change");
    Driver.end();
    lightToggle = true;
  }
}


void strobe() {
  if (strobeToggle) {
    strobeToggle = false;
  } else {
    strobeToggle = true;
  }
}

// Checks if strobe should be on. If so, stobes.
void strobeCheck() {
  if (strobeToggle) {
    if (strobeValue) {
        delay(10);
        Driver.begin(); // begin
        Driver.SetColor(0, 0, 0); // Set to RED Color
        Driver.SetColor(0, 0, 0); // Set to RED Color
        Driver.end();
        strobeValue = false; // low
    } else {
        delay(130);
        Driver.begin(); // begin
        Driver.SetColor(200, 200, 200); // Set to RED Color
        Driver.SetColor(200, 200, 200); // Set to RED Color
        Driver.end();
        strobeValue = true; // high
     }
   }
}

void lightsOff() {
  Driver.begin(); // begin
  Driver.SetColor(0, 0, 0); // Set to RED Color
  Driver.SetColor(0, 0, 0); // Set to RED Color
  Driver.end();
}

void purple() {
  Driver.begin(); // begin
  Driver.SetColor(100, 0, 100); // Set to RED Color
  Driver.SetColor(100, 0, 100); // Set to RED Color
  Driver.end();
}

void orange() {
  Driver.begin(); // begin
  Driver.SetColor(255, 140, 0); // Set to RED Color
  Driver.SetColor(255, 140, 0); // Set to RED Color
  Driver.end();
}
