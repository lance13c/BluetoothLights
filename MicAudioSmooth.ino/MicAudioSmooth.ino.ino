//Lights
#include "RGBdriver.h"
#define CLK 9//pins definitions for the driver        
#define DIO 8
RGBdriver Driver(CLK,DIO);

int DELAY;
int LOOPS;
bool PAUSE;

//Mic
int DO = 2; //Pin for Digital Output - DO
int DA = A0; // Pin for Analog Output - AO
int threshold = 532; //Set minimum threshold for LED lit
int sensorvalue = 0;
int preSensorValue = 0;
int level = 0;
int preLevel = 0;
int const MAX_LEVEL = 20;
int const MIN_LEVEL = 4;
int lightValue = 0;
int preLightValue = 0;
int const BRIGHTNESS = 200; // 0-255
bool bass = false;

unsigned int rgbColour[3];
 
void setup() {
  Serial.begin(9600);

  DELAY = 30; //Milli seconds
  LOOPS = 40; //How many time the lights flash
  PAUSE = false; //Stops the program from doing anything
  if (PAUSE){
    Driver.begin(); // begin
    Driver.SetColor(0, 0, 0); // Set to No Color
    Driver.end();
  }
}
 
void loop() {
  // Mic Sensor
  sensorvalue = int(analogRead(DA));  //Read the analog value
  //Serial.print("Threshold: ");
  //Serial.println(sensorvalue);
//  for (int i=12; i>12; i--){
//    if (sensorvalue > (preSensorValue+i)){
//      level = i;
//      //for (int j=1; j<i; j++){
//        //Serial.print("--------");
//      //}
//      //Serial.println("");
//      break;
//    }else{
//      level = 0;
//    }
//  }
//  bool trigger = false;
//  for (int i=MAX_LEVEL;i>MIN_LEVEL;i--){
//    if (sensorvalue > (preSensorValue+i)){
//      level = i;
//      Serial.println("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& IN HERE");
//      break;
//    }
//    Serial.print("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
//    Serial.println(i);
//  }
//  if (sensorvalue <= (preSensorValue+MIN_LEVEL)){
//      level = 0;
//  }
  if (sensorvalue > (preSensorValue+20)){
    level = 20;
  }else if(sensorvalue > (preSensorValue+19)){
    level = 19;
  }else if(sensorvalue > (preSensorValue+18)){
    level = 18;
  }else if(sensorvalue > (preSensorValue+17)){
    level = 17;
  }else if(sensorvalue > (preSensorValue+16)){
    level = 16;
  }else if(sensorvalue > (preSensorValue+15)){
    level = 15;
  }else if(sensorvalue > (preSensorValue+14)){
    level = 14;
  }else if(sensorvalue > (preSensorValue+13)){
    level = 13;
  }else if(sensorvalue > (preSensorValue+12)){
    level = 12;
  }else if(sensorvalue > (preSensorValue+11)){
    level = 11;
  }else if(sensorvalue > (preSensorValue+10)){
    level = 10;
  }else if(sensorvalue > (preSensorValue+9)){
    level = 9;
  }else if(sensorvalue > (preSensorValue+8)){
    level = 8;
  }else if(sensorvalue > (preSensorValue+7)){
    level = 7;
  }else if(sensorvalue > (preSensorValue+6)){
    level = 6;
  }else if(sensorvalue > (preSensorValue+5)){
    level = 5;
  }else if(sensorvalue > (preSensorValue+4)){
    level = 4;
  }else{
    level = 0;
  }
  preSensorValue = sensorvalue;

  //Lights
  lightValue = (BRIGHTNESS/MAX_LEVEL) * level;
  if (lightValue != 0){
    Serial.print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  ");
    Serial.println(lightValue);
  }
  if (lightValue != preLightValue){
    int r = random(3);
    Serial.print(r);
    Driver.begin(); // begin

    int incColour = decColour == 2 ? 0 : decColour + 1;
    rgbColour[decColour] -= 1;
    rgbColour[incColour] += 1;
    
    rgbColour[decColour] -= 1;
    rgbColour[incColour] += 1;
    
    switch (r){
      case 0:
        Driver.SetColor(0, lightValue, 0); //Green. first node data
        break;
      case 1:
        Driver.SetColor(lightValue, 0, 0); //Green. first node data
        break;
      case 2:
        Driver.SetColor(0, 0, lightValue); //Green. first node data
        break;
    }
    
    Driver.end();
    
    if (lightValue == 0){
      delay(10*preLevel);
      bass = false;
    }
    if (lightValue >= (BRIGHTNESS-10)){
      bass = true;
    }
    preLightValue = lightValue;
    preLevel = level;
  }
}
