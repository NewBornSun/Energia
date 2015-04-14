/**************************************************************/
// State of Health Algorithm
// Edited by Alex LePelch on Feb. 26th 2015
/**************************************************************/



/**************************************************************/
// Defines and Variables for Basic Hardware
/**************************************************************/

#define LED1 PN_1
#define LED2 PN_0
#define LED3 PF_4
#define LED4 PF_0

/**************************************************************/
// SD card(SPI bus 2) defines and variables
/**************************************************************/

// DAT0(MISO) - 14
// CMD (MOSI) - 15
// CLK        - 7
// CS         - 39 (PF2)

#include<SPI.h>
#include<SD.h>

File OCV_File;
File Valley_File;
File SOH_File;

const int chipselect = 39;
const char* SOH_Filename = "SOH.txt";
char Valley_Filename[13], OCV_Filename[13];
char linestringtemp[50], linestringtemp1[13], dataString[55], SOHstring[55], serialData[105];

int first = 0;
int SOHfirst = 1;
int flag = 0;
int saveValleyDet = 0;
int valleyFlag = 0;

/**************************************************************/
// Includes and defines for RTC
/**************************************************************/

#include "inc/tm4c1294ncpdt.h"
#include "inc/tm4c129xnczad.h"
#include "driverlib/hibernate.c"

tm *temp = new tm;
const int year = 15;

/**************************************************************/
// Defines and Variables for Interrupt
/**************************************************************/

#include <stdint.h>
#include <stdbool.h>

#define PART_TM4C1294NCPDT
#include "inc/tm4c1294ncpdt.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

const unsigned char OCV_Frequency = 1/1;    // Frequency for OCV
const unsigned char VDFreq = 200;           // Frequency for ValleyDetector

unsigned char OCV_Measure_Flag = 0; 

// Flags to trigger valley detect scans. 
// Zero means no scan, 1 means scan starting at 0,
// 2 means scan starting at middle
unsigned char ValleyDetect_Flag = 0;

unsigned long previousMillis = 0;
unsigned long starttime = 0;  // = micros();
unsigned long endtime = 0;    // = micros();

// Status definitions
#define initialized 0
#define VoltSettled 1
#define dvdtdetected 2      // ready for valley 1
#define valley1detected 3   // ready for valley 2
#define valley2detected 4   // ready for onThresh
#define onThreshpassed 5
#define SoHfailed 6
#define SoHpassed 7

unsigned char ValleyStatus = initialized;

/**************************************************************/
// Defines and Variables for Voltage Measurement
/**************************************************************/

// A0 is breakout board Vin sensing, A1 is pot, A2 is temperature
const int BatteryVoltagePin = A1;

float measvolts = 0;

char voltstring[8];
const float voltspercount = 12.00/3045;//12.01/2957;
const float millivoltspercount = 12000/3045;
const int countspervolt = 3045/12.00;

/**************************************************************/
// Temperature Sensor (1uF between Vout and Ground)
/**************************************************************/

const int TemperatureVoltagePin = A3;



/**************************************************************/
// END DEFINITIONS AND HEADERS
/**************************************************************/




/**************************************************************************/
// START SETUP(); 
/**************************************************************************/

void setup() //Run once
{
  Serial.begin(115200);
  
  /************************************************/
  // SD card init
  delay(1000);
  SPI.setModule(2); // SPI Bus 2
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(chipselect)) {
    Serial.println("initialization failed!");
    return;}
  
  Serial.print("Initialization done.\n");
  delay(1000);
  
  /************************************************/
  // RTC init
  // Enable Hibernate peripheral. I'm using Energia so i use F_CPU for geting the clock frequency
  HibernateEnableExpClk(F_CPU);
  HibernateRTCEnable();
  
  // We set a interrupt for the RTC after second. You can change the value
  HibernateRTCMatchSet(0,HibernateRTCGet()+1);
  HibernateIntRegister(HibernateHandler);
  HibernateIntEnable(HIBERNATE_INT_RTC_MATCH_0);
  
  // Set up calender mode. HibernateCounterMode() is always needed but can be set to 12hr mode
  HibernateCounterMode(HIBERNATE_COUNTER_24HR);
  HibernateCalendarSet(temp); //<-- the struct declared
  
  // We change the hour, minutes and seconds
  temp->tm_hour= 14;
  temp->tm_min= 36;
  temp->tm_sec = 30;
  temp->tm_mon = 04;
  temp->tm_mday = 04;
  temp->tm_year = 15;
  // This fuction below is what actualy updates the values inside the peripheral. 
  // if you don't use it, the value changes above won't do anyting
  HibernateCalendarSet(temp);
 
  CheckFileName(); //New file for each day
  Serial.print("\nOCV_Filename: ");
  Serial.print(OCV_Filename);
  Serial.println("\n");
 
  /************************************************/
  // Temperature Sesnor Init
  pinMode(TemperatureVoltagePin, INPUT);
  
  /************************************************/
  // LED indicator outputs
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  
  /************************************************/
  // Initialize Timer Interrupts
  initTimer0_OCV();
  initTimer1_Valley();
}

/*************************************************************************/
// END SETUP();
/*************************************************************************/




/*************************************************************************/
// START LOOP();
/*************************************************************************/
void loop()
{
  CheckFileName();
  
  if(OCV_Measure_Flag) OCV_Checker();
  
  if(ValleyDetect_Flag >= 1 && valleyFlag == 1)
  {
    Serial.println("Valley Interrupt");
    ValleyStatus = Valley_Processor(ValleyDetect_Flag);
    //if(saveValleyDet) SaveBurstToSD();
    SaveBurstToSD();
    ValleyDetect_Flag = 0;
  }
}

/*************************************************************************/
// END LOOP();
/*************************************************************************/



