/*
  
  Continuous Algorithm 4 implements 4-value averaging on the MCU. 
  this is an enhancement that was not implemented in matlab's Continuous Algorithm 3. 
  
  */


//Defines and Variables for Basic Hardware
#define LED1 PN_1
#define LED2 PN_0
#define LED3 PF_4
#define LED4 PF_0


/*
  Defines and Variables for SD card
 * SD card attached to SPI 1 bus as follows to SPI1:
 ** MOSI - pin 2
 ** MISO - pin 6
 ** CLK - pin 64
 ** CS - pin 3
 */
 
#include <SPI.h>
#include <SD.h>

const int chipSelect = 3;
File OCV_File;
File Valley_File;
byte zero = 0x00; //workaround for issue #527

char dataString[50];
char OCV_filename[32] = "00000000000000000-00-00.csv";
char Valley_Filename[32] = "00-00-00.csv";

int file_dayofmonth = 99;
int file_month = 99;
int file_year = 99;


//Defines and Variables for RTC Module
#include <Wire.h>
int seconds = 0;
int minutes = 0;
int hours = 0; //24 hour time
int dayofweek = 0; //0-6 -> sunday - Saturday   
int dayofmonth = 0;
int months = 0;
int years = 0;


//Defines and Variables for Interrupt
#include <stdint.h>
#include <stdbool.h>

#define PART_TM4C1294NCPDT
#include "inc/tm4c1294ncpdt.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

//Defines and Variables for Voltage Measurement

//A0 is breakout board Vin sensing, A1 is pot, A2 is temperature
const int BatteryVoltagePin = A0;

float measvolts = 0;

char voltstring[8];
const float voltspercount = 14.97/3788;
const float millivoltspercount = 14970.0/3788;
const int countspervolt = 3788/14.97;

//////////////////////Variables for Time and Interrupt Management
const unsigned char VDFreq = 200;  //Frequency for ValleyDetector

const unsigned char OCV_Frequency = 1/1;  //Frequency for OCV




unsigned long previousMillis = 0;

//char * linestring[VDSamples] = {};
char linestringtemp[50];


unsigned long starttime = 0;// = micros();
unsigned long endtime = 0;// = micros();

//unsigned char newburstdata = 0;
unsigned char OCV_Measure_Flag = 0;

//Flags to trigger valley detect scans. Zero means no scan, 1 means scan starting at 0, 2 means scan starting at middle
unsigned char ValleyDetect_Flag = 0;

//Status definitions
#define initialized 0
#define VoltSettled 1
#define dvdtdetected 2  //ready for valley 1
#define valley1detected 3  //ready for valley 2
#define valley2detected 4  //ready for onThresh
#define onThreshpassed 5
#define SoHfailed 6
#define SoHpassed 7
unsigned char ValleyStatus = initialized;

////////////////////////////////////////////////




void setup ()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.setModule(0);
  
  SPI.setModule(1);
  
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(3)) {
    Serial.println("initialization failed!");
    return;
  }
  GetTimeDate();
  CheckFileName();
  Serial.println("initialization done.");

  
  /*
  Serial.print("millivoltspercount = ");
  Serial.println(millivoltspercount,10);
  Serial.print("voltspercount = ");
  Serial.println(voltspercount,10);
  Serial.print("countspervolt = ");
  Serial.println(countspervolt);
  */
  
  
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  
  initTimer0_OCV();
  initTimer1_Valley();
  
  
}

void loop()
{ 
  CheckFileName();

  if(OCV_Measure_Flag)
  {
    OCV_checker();
  }
  
  
  if(ValleyDetect_Flag >= 1)
  {
    ValleyStatus = Valley_Processor(ValleyDetect_Flag);
    SaveBurstToSD();
    ValleyDetect_Flag = 0;
  }
  

}



