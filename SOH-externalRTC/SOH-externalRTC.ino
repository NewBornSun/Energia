/**************************************************************/
// State of Health Algorithm
// Edited by Alex LePelch on May 7th 2015
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
char Valley_Filename[13], OCV_Filename[13];
char linestringtemp[50], linestringtemp1[13], dataString[55], SOHstring[55], serialData[105];

int first = 0;
int SOHfirst = 1;
int flag = 0;
int saveValleyDet = 0;
int OCV_Flag = 0;
int SoCflag = 0;

int file_dayofmonth = 99;
int file_month = 99;
int file_year = 99;

/**************************************************************/
// Includes and defines for RTC
/**************************************************************/

//#include "inc/tm4c1294ncpdt.h"
//#include "inc/tm4c129xnczad.h"
//#include "driverlib/hibernate.c"
#include <Wire.h>

int seconds = 0;
int minutes = 0;
int hours = 0; //24 hour time
int dayofweek = 0; //0-6 -> sunday - Saturday   
int dayofmonth = 0;
int months = 0;
int years = 0;

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

unsigned char status = initialized;
unsigned char ValleyStatus = initialized;

/**************************************************************/
// Defines and Variables for Voltage Measurement
/**************************************************************/

const int BatteryVoltagePin = A1;

float measvolts = 0;

char voltstring[8];
const float voltspercount = 12.00/3023;
const float millivoltspercount = 12000/3023;
const int countspervolt = 3023/12.00;

/**************************************************************/
// Temperature Sensor (1uF between Vout and Ground)
/**************************************************************/

const int TemperatureVoltagePin = A3;

/**************************************************************/
// EEPROM and GPIO defines and variables
/**************************************************************/

/*#include "driverlib/eeprom.h"
#include "driverlib/gpio.h"

uint32_t eepromReadData;
uint32_t eepromWriteData = 150;
uint32_t restoreEEPROM = 0;
uint32_t EEPROM_TEST_ADDRESS = 0x0000;
int gpioInt = 1;

/**************************************************************/
// Defines and variables for SoH determination.
// Must be global so that SaveSoHtoSD() has access.
/**************************************************************/

 double OCV_Volts;
 double V1_Volts;
 double V2_Volts;
 double deltaV1;
 double deltaV2_Volts;
 
 double VTH1;
 double VTH2;
 double VTH3;
 double Vth;
 double SoH_Metric;
 double temperature_Celsius;
 unsigned int SoC_recent;
 
 int Pass = 0;
 int Warning = 0;
 int SoH_check = 0;
 unsigned int  SoC_Max = 0;
 
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
  // RTC init
  Wire.begin();
  Wire.setModule(0); //I2C bus 0
 
  CheckFileName(); //New file for each day
  Serial.print("\nOCV_Filename: ");
  Serial.print(OCV_Filename);
  Serial.println("\n");
  
  /************************************************/
  // SD card init
  SPI.setModule(2); // SPI Bus 2
  
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(chipselect)) {
    Serial.println("initialization failed!");
    return;}
  
  Serial.print("Initialization done.\n");
 
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
  
  /************************************************/
  // EEPROM Init
  /*SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
  
  if(EEPROMInit() == EEPROM_INIT_OK) Serial.println("EEPROM_INIT_OK");
  else if(EEPROMInit() == EEPROM_INIT_RETRY) Serial.println("EEPROM_INIT_RETRY");
  else if(EEPROMInit() == EEPROM_INIT_ERROR) Serial.println("EEPROM_INIT_ERROR");
  else Serial.println("None of the above");
   
  /************************************************/
  // GPIO interrupt init
  /*GPIODirModeSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_DIR_MODE_IN);
  GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
  GPIOIntRegister(GPIO_PORTM_BASE, GPIOIsr);
  GPIOIntTypeSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
  GPIOIntEnable(GPIO_PORTM_BASE, GPIO_INT_PIN_5);*/
  
  //******************************************************//
// This interrupt is caused by a low reading on pin 30
//******************************************************//

/*void GPIOIsr()
{
  Serial.println("Interrupt Handler");
  EEPROMProgram((uint32_t *)&eepromWriteData, EEPROM_TEST_ADDRESS, 4);
  
  GPIOIntClear(GPIO_PORTM_BASE, GPIO_INT_PIN_5);
}
 */  
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
  
  if(ValleyDetect_Flag >= 1 && OCV_Flag == 1)
  {
    Serial.println("Valley Interrupt");
    ValleyStatus = Valley_Processor(ValleyDetect_Flag);
    if(status >= dvdtdetected) SaveBurstToSD();
    if(SoH_check)
    { 
      SaveSOHtoSD(Pass, V1_Volts, V2_Volts, OCV_Volts, temperature_Celsius, SoH_Metric, SoC_Max, Warning);
      SoH_check = 0;
    }
    ValleyDetect_Flag = 0;
  }
}

/*************************************************************************/
// END LOOP();
/*************************************************************************/



