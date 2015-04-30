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

uint32_t eepromReadData;
uint32_t eepromWriteData = 59;
uint32_t restoreEEPROM = 0;
uint32_t EEPROM_TEST_ADDRESS = 0x0000;

void setup()
{
  Serial.begin(115200);
  
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  pinMode(LED3,OUTPUT);
  pinMode(LED4,OUTPUT);
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  
  if(EEPROMInit() == EEPROM_INIT_OK) Serial.println("EEPROM_INIT_OK");
  else if(EEPROMInit() == EEPROM_INIT_RETRY) Serial.println("EEPROM_INIT_RETRY");
  else if(EEPROMInit() == EEPROM_INIT_ERROR) Serial.println("EEPROM_INIT_ERROR");
  else Serial.println("None of the above");
  
  SysCtlIntEnable(SYSCTL_INT_BOR);
  IntRegister(SYSCTL_INT_BOR, BORIsr);
  
  
  SysCtlResetBehaviorSet(SYSCTL_ONRST_BOR_SYS);
  SysCtlVoltageEventConfig(SYSCTL_VEVENT_VDDCBO_INT |
                           SYSCTL_VEVENT_VDDABO_INT |
                           SYSCTL_VEVENT_VDDBO_INT);
  
  if(SysCtlResetCauseGet() == SYSCTL_CAUSE_LDO)
  {
    Serial.println("SYSCTL_CAUSE_LDO");
    digitalWrite(LED3, HIGH);
  }
  else if(SysCtlResetCauseGet() == SYSCTL_CAUSE_SW) Serial.println("SYSCTL_CAUSE_SW");
  else if(SysCtlResetCauseGet() == SYSCTL_CAUSE_WDOG) Serial.println("SYSCTL_CAUSE_WDOG");
  else if(SysCtlResetCauseGet() == SYSCTL_CAUSE_BOR)
  {
    Serial.println("SYSCTL_CAUSE_BOR");
    digitalWrite(LED1, HIGH);
  }
  else if(SysCtlResetCauseGet() == SYSCTL_CAUSE_POR)
  {
    Serial.println("SYSCTL_CAUSE_POR");
    digitalWrite(LED2, HIGH);
  }
  else if(SysCtlResetCauseGet() == SYSCTL_CAUSE_EXT) Serial.println("SYSCTL_CAUSE_EXT");
  else
  {
    Serial.println("None");
    digitalWrite(LED4,HIGH);
  }
  
  SysCtlResetCauseClear(SYSCTL_CAUSE_LDO|SYSCTL_CAUSE_SW|SYSCTL_CAUSE_WDOG|SYSCTL_CAUSE_BOR|SYSCTL_CAUSE_POR|SYSCTL_CAUSE_EXT);
  
}

void loop()
{
  //digitalWrite(LED4,HIGH); // put your main code here, to run repeatedly:
  
 
  
}

void BORIsr()
{
  EEPROMProgram((uint32_t *)&eepromWriteData, EEPROM_TEST_ADDRESS, 4);
  SysCtlIntClear(SYSCTL_INT_BOR);
  //digitalWrite(LED3, HIGH);
}
