/**************************************************************/
//State of Health Algorithm
//Edited by Alex LePelch on Feb. 26th 2015
/**************************************************************/



/**************************************************************/
//Defines and Variables for Basic Hardware
#define LED1 PN_1
#define LED2 PN_0
#define LED3 PF_4
#define LED4 PF_0

int count;

/**************************************************************/
//SD card(SPI bus 2) defines and variables
// DAT0(MISO) - 14
// CMD (MOSI) - 15
// CLK        - 7
// CS         - 63

#include<SPI.h>
#include<SD.h>

File OCV_File;
File Valley_File;

char Valley_Filename[13], OCV_Filename[13];
char linestringtemp[50], linestringtemp1[13];

int file_dayofmonth = 99;
int file_month = 99;
int file_year = 99;

int counter = 0;
int reading = 0;
int first = 0;
double readVoltage[100];
double readTemperature[100];
byte linestring[50];

/**************************************************************/
//Includes and defines for RTC
#include <Wire.h>

int clockAddress = 0x68;  // This is the I2C address
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

/**************************************************************/
//Defines and Variables for Interrupt
#include <stdint.h>
#include <stdbool.h>

#define PART_TM4C1294NCPDT
#include "inc/tm4c1294ncpdt.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

const unsigned char OCV_Frequency = 1/1;  //Frequency for OCV
const unsigned char VDFreq = 200;  //Frequency for ValleyDetector

unsigned char OCV_Measure_Flag = 0; 

//Flags to trigger valley detect scans. 
//Zero means no scan, 1 means scan starting at 0,
//2 means scan starting at middle
unsigned char ValleyDetect_Flag = 0;

/**************************************************************/
//Defines and Variables for Voltage Measurement
//A0 is breakout board Vin sensing, A1 is pot, A2 is temperature
const int BatteryVoltagePin = A0;

float measvolts = 0;

char voltstring[8];
const float voltspercount = 14.97/3788;
const float millivoltspercount = 14970.0/3788;
const int countspervolt = 3788/14.97;

/**************************************************************/
//Temperature Sensor
const int TemperatureVoltagePin = A2;
double celcius;



void setup() //Run once
{
  Serial.begin(115200);
  
  /************************************************/
  //SD card init
  SPI.setModule(2); //SPI Bus 2
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(42)) {
    Serial.println("initialization failed!");
    return;}
  
  Serial.print("Initialization done.\n");
  
  /************************************************/
  //RTC init
  //Wire.begin(); //RTC I2C (causes problems)
  Wire.setModule(0); //I2C bus 0
  GetTimeDate();
  CheckFileName(); //New file for each day
 
  /************************************************/
  //Temperature Sesnor Init
  pinMode(TemperatureVoltagePin, INPUT);
  
  /************************************************/
  //LED indicator outputs
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  
  /************************************************/
  //Initialize Timers
  //initTimer0_OCV();
  //initTimer1_Valley();
}

void loop()
{
  //CheckFileName();
  
  //if(OCV_Measure_Flag) OCV_Checker;
  double voltage, voltage2;
 
  while(counter < 21) //Take 20 readings, store into SD card
  {
    voltage = analogRead(BatteryVoltagePin);
    voltage2 = voltage*3.3/4096;
    celcius = Measure_Temperature();
    
    OCV_File = SD.open(OCV_Filename, FILE_WRITE);
  
    if (OCV_File) //Check to see if we can open file
    {
      Serial.print("Writing to");
      Serial.print(OCV_Filename);
      if(!first) //If its the first time, write to top of file
      {
        OCV_File.println("Voltage, Temperature(C)"); //Comma separated values
        first = 1;
        OCV_File.close();
      }
        
      else //For every other write following the first, write the data
      {
        sprintf(linestringtemp, "%9f,%04f", voltage2, celcius); //Format string
        OCV_File.println(linestringtemp);
        OCV_File.close(); //Close the file
      }
      
      Serial.println(linestringtemp);  
    }         
    else  Serial.println("error opening test.csv"); //print an error if file doesn't open
    counter++;
  }

    
}


