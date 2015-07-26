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
int const MAX_LEVEL = 15;
int lightValue = 0;
int preLightValue = 0;
int const BRIGHTNESS = 150; // 0-255
bool bass = false;
 
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
  if (sensorvalue > (preSensorValue+15)){
    level = 15;
  }else if(sensorvalue > (preSensorValue+13)){
    level = 13;
  }else if(sensorvalue > (preSensorValue+11)){
    level = 11;
  }else if(sensorvalue > (preSensorValue+9)){
    level = 9;
//  }else if(sensorvalue > (preSensorValue+7)){
//    level = 7;
//  }else if(sensorvalue > (preSensorValue+5)){
//    level = 5;
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
    Driver.begin(); // begin
    Driver.SetColor(0, lightValue, 0); //Red. first node data
    Driver.end();
    
    if (bass && lightValue == 0){
      Driver.begin(); // begin
      //Driver.SetColor(0, 0, 128); //Red. first node data
      delay(200);
      Driver.end();
      bass = false;
    }
    if (lightValue > (BRIGHTNESS)){
      bass = true;
    }
    preLightValue = lightValue;
  }
}
