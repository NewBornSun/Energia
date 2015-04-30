#include <stdint.h>
#include <stdbool.h>

#include<SPI.h>
#include<SD.h>

#define PART_TM4C1294NCPDT
#include "inc/tm4c1294ncpdt.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/eeprom.h"

#define LED1 PN_1
#define LED2 PN_0
#define LED3 PF_4
#define LED4 PF_0

File Valley_File;

uint32_t eepromReadData;
uint32_t eepromWriteData = 109;
uint32_t restoreEEPROM = 0;
uint32_t EEPROM_TEST_ADDRESS = 0x0000;

int first = 1;
const int chipselect = 39;

void setup()
{
  Serial.begin(115200);
  
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  //pinMode(30, INPUT);
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  
  
  GPIODirModeSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_DIR_MODE_IN);
  GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
  GPIOIntRegister(GPIO_PORTM_BASE, GPIOIsr);
  GPIOIntTypeSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
  GPIOIntEnable(GPIO_PORTM_BASE, GPIO_INT_PIN_5);
  //
  
  //IntEnable(INT_GPIOM);
  SysCtlIntRegister(GPIOIsr);
  SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOM);
  SysCtlPeripheralClockGating(true);
  SysCtlIntEnable(INT_GPIOM);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
  
  if(EEPROMInit() == EEPROM_INIT_OK) Serial.println("EEPROM_INIT_OK");
  else if(EEPROMInit() == EEPROM_INIT_RETRY) Serial.println("EEPROM_INIT_RETRY");
  else if(EEPROMInit() == EEPROM_INIT_ERROR) Serial.println("EEPROM_INIT_ERROR");
  else Serial.println("None of the above");

  Serial.println("Begin loop()");
}

void loop()
{ 
//  EEPROMRead((uint32_t *)&eepromReadData, EEPROM_TEST_ADDRESS, 4); // Read Contents
//  
//  if(eepromReadData == 111)
//  {
//    digitalWrite(LED1, HIGH);
//    EEPROMProgram((uint32_t *)&restoreEEPROM, EEPROM_TEST_ADDRESS, 4);
//  }
  digitalWrite(LED3, HIGH);
  delay(1000);
  digitalWrite(LED3, LOW);
  delay(1000);
}

void GPIOIsr()
{
  Serial.println("Interrupt Handler");
  digitalWrite(LED1, HIGH);
  EEPROMProgram((uint32_t *)&eepromWriteData, EEPROM_TEST_ADDRESS, 4);
  GPIOIntClear(GPIO_PORTM_BASE, GPIO_INT_PIN_5);
  //SysCtlSleep();
}
