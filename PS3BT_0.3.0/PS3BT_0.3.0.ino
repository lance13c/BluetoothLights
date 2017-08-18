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

String hero = ("    ____  __           __              __  __       \n"
"   / __ )/ /_  _____  / /_____  ____  / /_/ /_      \n"
"  / __  / / / / / _ \/ __/ __ \/ __ \/ __/ __ \     \n"
" / /_/ / / /_/ /  __/ /_/ /_/ / /_/ / /_/ / / /     \n"
"/_____/_/\__,_/\____\__/_____/\____/\__/_/ /_/      \n"
"   / /   (_)___ _/ /_  / /______                    \n"
"  / /   / / __ `/ __ \/ __/ ___/                    \n"
" / /___/ / /_/ / / / / /_(__  )                     \n"
"/_____/_/\__, /_/ /_/\__/____/                      \n"
"        /____/                        \n");

String hero2 = ("  ___ _          _            _   _     \n"
" | _ ) |_  _ ___| |_ ___  ___| |_| |_           \n"
" | _ \ | || / -_)  _/ _ \/ _ \  _| ' \          \n"
" |___/_|\_,_\___|\__\___/\___/\__|_||_|         \n"
" | |  (_)__ _| |_| |_ ___                       \n"
" | |__| / _` | ' \  _(_-<                       \n"
" |____|_\__, |_||_\__/__/                       \n"
"        |___/");

int DELAY;
int LOOPS;
bool PAUSE;
bool lightToggle;
bool strobeToggle;              // Wheather to strobe
bool strobeValue;               // If off or on
String device;                  // Either "TEST" or "CTRL"  (Stands for testing or controller)
int masterRGB[] = {0, 0, 0};      // The actual color being represented;
int currentRGB[] = {0, 0, 0};     // The color actually being displayed with all augments
int tempRGB[] = {0, 0, 0};
float initCtrlAngle = -1.0;       // Init angle for Controller Roll and Pitch 
int tempCount = 0;

float brightnessMul = 1.0;      // Brightness Multiplier
float tempBrightnessMul = brightnessMul;  // Temp brightness multiplyer for brightness changes;

// MOOD LIGHT VARS
int colorIndex = 0;
const int COLOR_MAX_INDEX = 13;
int requestColorChange = true;                          // TODO FIGURE OUT MOOD LIGHTS
int tColorR = 0;  // Transitioning Color RED
int tColorG = 0;  // Transitioning Color GREEN
int tColorB = 0;  // Transitioning Color BLUE
int colorTransitionTime = 10;  // Arbitray number, used for mapping color difference values
                               // EX: tColorG = 10, colorG = 40, 40-10 = 30, Map(val,0,30,0,10)
int colorMap[COLOR_MAX_INDEX][3] = {
  {252,5,0},    // 0
  {245,97,1},   // 1
  {239,146,0},  // 2
  {254,240,5},  // 3
  {178,237,1},  // 4
  {19,227,1},   // 5
  {7,218,140},  // 6
  {0,236,220},  // 7
  {1,145,206},  // 8
  {2,55,211},   // 9
  {79,12,118},  // 10
  {113,12,83},  // 11
  {157,5,26}};  // 12

#define C_RED 0
#define C_ORANGE_RED 1
#define C_ORANGE 2
#define C_YELLOW 3
#define C_YELLOW_GREEN 4
#define C_GREEN 5
#define C_GREEN_AQUA 6
#define C_AQUA 7
#define C_AQUA_BLUE 8
#define C_BLUE 9
#define C_PURPLE 10
#define C_PURPLE_RED 11
#define C_BLOOD_RED 12

#define TEST_R_PIN 44
#define TEST_G_PIN 46
#define TEST_B_PIN 45

#define CTRL_R_PIN 2
#define CTRL_G_PIN 3
#define CTRL_B_PIN 4

// Device Map
#define DEVICE_TEST "TEST"
#define DEVICE_CTRL "CTRL"

// Feature Map
#define BLACK_OUT 0
#define IMPACT 1
boolean featureMap[] = {false, false};

// Effect Map
#define STROBE 0
#define MOOD 1
#define ADJUST_BRIGHTNESS 2
boolean effectMap[] = {false, false, false};

boolean lightLockEnable = false;          // Determines if the lights are allowed to change.    

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
PS3BT PS3(&Btd); // This will just create the instance
//PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

bool printTemperature;
bool printAngle = 0;

void setup() {
  Serial.begin(115200);

  //TEST AREA
  device = DEVICE_TEST;
  toggleFeature("BLACK_OUT");

  //setRGBTests();
  //blackOutTest();


  //END TEST AREA

  purple();
  //setRGB(device, 200, 200, 200, false, false);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  String temp = "\r\n" + hero;
  Serial.println(hero2);
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
        //purple();
        Serial.print(F("\r\nTriangle"));
        PS3.moveSetBulb(Red);
      }
      if (PS3.getButtonPress(CIRCLE)) {
        //strobe();
        //lightsOff();
        // TOGGLES THE EFFECT
        //toggleEffect("ADJUST_BRIGHTNESS");
        //Serial.print(F("\r\nCircle"));
        //PS3.moveSetBulb(Green);
      }
      if (PS3.getButtonClick(CIRCLE)) {
        // TOGGLES THE EFFECT
        toggleEffect("ADJUST_BRIGHTNESS");
        //Serial.print(F("\r\nCircle"));
      }
      if (PS3.getButtonClick(SQUARE)) {
        //orange();
        Serial.print(F("\r\nSquare"));
        PS3.moveSetBulb(Blue);
      }
      if (PS3.getButtonClick(CROSS)) {
        Serial.print(F("\r\nCross"));
        PS3.moveSetBulb(Yellow);
      }
      if (PS3.getButtonClick(MOVE)) {

        //toggleLights();
        
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

   // EFFECT CHECKING

   //effectMap[ADJUST_BRIGHTNESS] && 
   if (PS3.getButtonPress(CIRCLE)){
      adjustBrightness();
   } // Continue with else ifs


    
  }
#endif
}

// Only one effect can run at once.
void toggleEffect(String effect) {
  boolean val;
  if (effect == "ADJUST_BRIGHTNESS"){
    setInitCtrlAngle();
    val = effectMap[ADJUST_BRIGHTNESS];
    effectMap[ADJUST_BRIGHTNESS] = (val) ? false : true;
    Serial.print("ADJUST_BRIGHTNESS TOGGLE: ");
    Serial.println(val);
  } else if (effect == "MOOD"){
    setInitCtrlAngle();
    val = effectMap[MOOD];
    effectMap[MOOD] = (val) ? false : true;
    Serial.print("ADJUST_BRIGHTNESS TOGGLE: ");
    Serial.println(val);
  } else {
    Serial.println("Effect Not Found: " + effect); 
  }
}


// Feature are effects that can run
//     at the same time other features & effects are running.
void toggleFeature(String feature) {
  boolean val;
  if (feature == "BLACK_OUT") {
    val = featureMap[BLACK_OUT];
    featureMap[BLACK_OUT] = (val) ? false : true;
  } else if (feature == "IMPACT") {
    val = featureMap[IMPACT];
    featureMap[IMPACT] = (val) ? false : true;
  } else {
    Serial.println("Feature Not Found: " + feature); 
  }
}

/**
 * toggle - If blackout should be toggled on or off.
 */
String blackOut(boolean toggle) {
  if (toggle) {
    tempRGB[0] = masterRGB[0];
    tempRGB[1] = masterRGB[1];
    tempRGB[2] = masterRGB[2];
    setRGB(device, 0, 0, 0, false, false);
    lightLockEnable = true;
    return "Blackout true";
  } else {
    setRGB(device, tempRGB[0], tempRGB[1], tempRGB[2], true, false);
    lightLockEnable = false;
    return "Blackout false";
  }
}

String setRGB(String device, int r, int g, int b, boolean overrideLock, boolean overrideBrightness){
  float localBrightness = (!overrideBrightness) ? brightnessMul : 1.0;
  float localR = r * localBrightness;
  float localG = g * localBrightness;
  float localB = b * localBrightness;
  //Serial.print("Brightness: ");
  //Serial.println(localBrightness);

  
  if (!lightLockEnable || overrideLock) {
    setMasterRGB(r, g, b);
    setCurrentRGB((int)localR, (int)localG, (int)localB);
    if (device == DEVICE_TEST) {
      analogWrite(TEST_R_PIN, (int)localR);
      analogWrite(TEST_G_PIN, (int)localG);
      analogWrite(TEST_B_PIN, (int)localB);
      return "Device Test";
    } else if (device == DEVICE_CTRL) {
      return "Device Ctrl";
    } else {
      return "No device found";
    }
  } else {
    return "Lights Locked";
  }
}

// Always use setInitCtrlAngle before & after using this function
// Change Brightness based on roll or pitch of controller
void adjustBrightness(){
  int roll = int(PS3.getAngle(Roll));
  //Serial.println("ROLL VALUE: " + roll);
  int diff = roll - int(initCtrlAngle);
  // Smooths out output
  int near = diff % 4;
  diff -= near;
  diff = diff;

  float scaledDiff = diff * 0.01;

  // Change global brightness multiplyer
  float newBrightness = (tempBrightnessMul + scaledDiff);
  if (newBrightness > 1.0) {newBrightness = 1.0;}
  if (newBrightness < 0.0) {newBrightness = 0.0;}
  Serial.print("New Brightness");
  Serial.println(newBrightness);
  brightnessMul = newBrightness;

  
  // End of smoothing
  
//  Serial.print("DIFF VALUE: ");
//  Serial.print(diff);
//  Serial.print("\n");
//  Serial.println("RED VALUE BEFORE: ");
//  Serial.print(currentRGB[0]);
//  Serial.print("\n");
  int r = (tempRGB[0]);
  int g = (tempRGB[1]);
  int b = (tempRGB[2]);
//
//  if (r >= 255) {r = 255;}
//  if (g >= 255) {g = 255;}
//  if (b >= 255) {b = 255;}
//
//  if (r <= 0) {r = 0;}
//  if (g <= 0) {g = 0;}
//  if (b <= 0) {b = 0;}
//
  setRGB(device, r, g, b, true, false);
//  Serial.print("RED VALUE: ");
//  Serial.print(r);
//  Serial.print("\n");
}

//void adjustBrightnessOnTrigger(int val) {
//}  

// Todo add alternative angle selection
void setInitCtrlAngle(){
  initCtrlAngle = PS3.getAngle(Roll);
  tempBrightnessMul = brightnessMul;
  setTempColorToMaster();
  Serial.println("Set Init Ctrl Angle");
}

void setTempColorToMaster() {
  tempRGB[0] = masterRGB[0];
  tempRGB[1] = masterRGB[1];
  tempRGB[2] = masterRGB[2];
}

void setMasterRGB(int r, int g, int b){
  masterRGB[0] = r;
  masterRGB[1] = g;
  masterRGB[2] = b;
}

void setCurrentRGB(int r, int g, int b){
  currentRGB[0] = r;
  currentRGB[1] = g;
  currentRGB[2] = b;
}

void purple() {
  setRGB(device, 100, 20, 200, false, false);
}

void moodLights() {
  if (colorIndex > COLOR_MAX_INDEX){
    colorIndex = 0;
  }
  int currentColorR = colorMap[colorIndex][0];
  int currentColorG = colorMap[colorIndex][1];
  int currentColorB = colorMap[colorIndex][2];

  int rDiff = currentRGB[0] - currentColorR;
  
}

void initMoodLights() {
  
}

void orange() {
  
}
