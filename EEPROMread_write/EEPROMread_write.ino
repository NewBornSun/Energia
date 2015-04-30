#include <stdint.h>
#include <stdbool.h>

#define PART_TM4C1294NCPDT
#include "inc/tm4c1294ncpdt.h"

#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/eeprom.h"

#define EEPROM_TEST_ADDRESS 0x0000 

uint32_t eepromReadData;
uint32_t eepromWriteData = 0;

void setup()
{
  Serial.begin(115200);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
  
  if(EEPROMInit() == EEPROM_INIT_OK) Serial.println("EEPROM_INIT_OK");
  else if(EEPROMInit() == EEPROM_INIT_RETRY) Serial.println("EEPROM_INIT_RETRY");
  else if(EEPROMInit() == EEPROM_INIT_ERROR) Serial.println("EEPROM_INIT_ERROR");
  else Serial.println("None of the above");
  
  //EEPROMProgram((uint32_t *)&eepromWriteData, EEPROM_TEST_ADDRESS, 4);
  
  EEPROMRead((uint32_t *)&eepromReadData, EEPROM_TEST_ADDRESS, 4);
  
  delay(1000);
  
  Serial.println(eepromReadData);
}

void loop()
{
  // put your main code here, to run repeatedly:
  
}
