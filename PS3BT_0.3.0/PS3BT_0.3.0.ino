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
float moodSpeed = 0.5;
float tempMoodSpeed = moodSpeed;
int colorIndex = 0;
const int COLOR_MAX_INDEX = 12;
int requestColorChange = true;                          // TODO FIGURE OUT MOOD LIGHTS
int tColorR = 0;  // Transitioning Color RED
int tColorG = 0;  // Transitioning Color GREEN
int tColorB = 0;  // Transitioning Color BLUE
int colorMap[COLOR_MAX_INDEX + 1][3] = {
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

int currentMoodColor[3] = {
  colorMap[colorIndex][0],
  colorMap[colorIndex][1],
  colorMap[colorIndex][2]};

int nextMoodColor[3] = {
  colorMap[colorIndex + 1][0],
  colorMap[colorIndex + 1][1],
  colorMap[colorIndex + 1][2]};

int diffMoodColor[3] = {0, 0, 0}; // Difference between current and next mood colors

float MAX_MOOD_VALUE = 1500;
float changeMoodValue = MAX_MOOD_VALUE;

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
#define MOOD_SPEED 3
boolean effectMap[] = {false, false, false, false};

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

  //purple();
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
        toggleEffect("MOOD_SPEED");
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
        toggleEffect("MOOD");
      }
      if (PS3.getButtonClick(MOVE)) {        
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
   } else if (PS3.getButtonPress(TRIANGLE)) {
      adjustMoodSpeed();
   }
   

   if (effectMap[MOOD]){
      moodLights();
   }
  }
#endif
}

// Only one effect can run at once.
void toggleEffect(String effect) {
  boolean val;
  if (effect == "ADJUST_BRIGHTNESS"){
    tempBrightnessMul = brightnessMul;
    setInitCtrlAngle();
    val = effectMap[ADJUST_BRIGHTNESS];
    effectMap[ADJUST_BRIGHTNESS] = (val) ? false : true;
    Serial.print("ADJUST_BRIGHTNESS TOGGLE: ");
    Serial.println(val);
  } else if (effect == "MOOD"){
    val = effectMap[MOOD];
    effectMap[MOOD] = (val) ? false : true;
    Serial.print("MOOD LIGHT TOGGLE: ");
    Serial.println(val);
  } else if (effect == "MOOD_SPEED"){
    tempMoodSpeed = moodSpeed;
    setInitCtrlAngle();
    val = effectMap[MOOD_SPEED];
    effectMap[MOOD_SPEED] = (val) ? false : true;
    Serial.print("MOOD SPEED TOGGLE: ");
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
  
  int r = (tempRGB[0]);
  int g = (tempRGB[1]);
  int b = (tempRGB[2]);

  setRGB(device, r, g, b, true, false);
} 

// Todo add alternative angle selection
void setInitCtrlAngle(){
  initCtrlAngle = PS3.getAngle(Roll);
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
  int r = 0;
  int g = 0;
  int b = 0;

  if (currentMoodColor[0] >= nextMoodColor[0]) {
    r = map(changeMoodValue, 0, MAX_MOOD_VALUE, nextMoodColor[0], currentMoodColor[0]);
  } else {
    r = map(changeMoodValue, 0, MAX_MOOD_VALUE, currentMoodColor[0], nextMoodColor[0]);  //c = 10, n = 20  20,19,18   n-c = 10; + n-r
    r = currentMoodColor[0] + (nextMoodColor[0] - r);
  }

  if (currentMoodColor[1] >= nextMoodColor[1]) {
    g = map(changeMoodValue, 0, MAX_MOOD_VALUE, nextMoodColor[1], currentMoodColor[1]);
  } else {
    g = map(changeMoodValue, 0, MAX_MOOD_VALUE, currentMoodColor[1], nextMoodColor[1]);  //c = 10, n = 20  20,19,18   n-c = 10; + n-r
    g = currentMoodColor[1] + (nextMoodColor[1] - g);
  }

  if (currentMoodColor[2] >= nextMoodColor[2]) {
    b = map(changeMoodValue, 0, MAX_MOOD_VALUE, nextMoodColor[2], currentMoodColor[2]);
  } else {
    b = map(changeMoodValue, 0, MAX_MOOD_VALUE, currentMoodColor[2], nextMoodColor[2]);  //c = 10, n = 20  20,19,18   n-c = 10; + n-r
    b = currentMoodColor[2] + (nextMoodColor[2] - b);
  }
//  Serial.print("Mood Light Colors: ");
//  Serial.print(r);
//  Serial.print(", ");
//  Serial.print(g);
//  Serial.print(", ");
//  Serial.println(b);

  setRGB(device, r, g, b, false, false);
  
  changeMoodValue -= moodSpeed;
  if (changeMoodValue <= 0) {
    moodColorChange();
  }
}

void moodColorChange() {
  colorIndex ++;
  if (colorIndex > COLOR_MAX_INDEX){
    colorIndex = 0;
  }

  int nextColorIndex = colorIndex + 1;
  if (nextColorIndex > COLOR_MAX_INDEX) {nextColorIndex = 0;}
  
  currentMoodColor[0] = colorMap[colorIndex][0];
  currentMoodColor[1] = colorMap[colorIndex][1];
  currentMoodColor[2] = colorMap[colorIndex][2];

  nextMoodColor[0] = colorMap[nextColorIndex][0];
  nextMoodColor[1] = colorMap[nextColorIndex][1];
  nextMoodColor[2] = colorMap[nextColorIndex][2];

  diffMoodColor[0] = nextMoodColor[0] - currentMoodColor[0];
  diffMoodColor[1] = nextMoodColor[1] - currentMoodColor[1];
  diffMoodColor[2] = nextMoodColor[2] - currentMoodColor[2];

  changeMoodValue = MAX_MOOD_VALUE;
  Serial.print("Color Change To: ");
  Serial.print(nextMoodColor[0]);
  Serial.print(", ");
  Serial.print(nextMoodColor[1]);
  Serial.print(", ");
  Serial.println(nextMoodColor[2]);
}

// Must call method setInitCtrlAngle Before use
void adjustMoodSpeed() {
  int roll = int(PS3.getAngle(Roll));
  int diff = roll - int(initCtrlAngle);
  // Smooths out output
  int near = diff % 4;
  diff -= near;
  diff = diff;

  float scaledDiff = diff * 0.01;

  // Change global brightness multiplyer
  float newMoodSpeed = (tempMoodSpeed + scaledDiff);
  if (newMoodSpeed > 1.0) {newMoodSpeed = 1.0;}
  if (newMoodSpeed < 0.0) {newMoodSpeed = 0.0;}
  Serial.print("Mood Speed: ");
  Serial.println(newMoodSpeed);
  moodSpeed = newMoodSpeed;
}

void orange() {
  
}

// Sets a1 values equal to a2 values
// src - int[3]
// src - int[3]
void copy(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}

