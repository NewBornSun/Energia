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

#define LED1 PN_1
#define LED2 PN_0
#define LED3 PF_4
#define LED4 PF_0

File Valley_File;

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
  GPIOIntTypeSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_BOTH_EDGES);
  GPIOIntEnable(GPIO_PORTM_BASE, GPIO_INT_PIN_5);
  //
  
  //IntEnable(INT_GPIOM);
  SysCtlIntRegister(GPIOIsr);
  SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOM);
  SysCtlPeripheralClockGating(true);
  SysCtlIntEnable(INT_GPIOM);
  
  delay(1000);
  SPI.setModule(2); // SPI Bus 2
  Serial.print("Initializing SD card...");
  
  if (!SD.begin(chipselect)) {
    Serial.println("initialization failed!");
    return;}
  
  else digitalWrite(LED4,HIGH);//Serial.print("Initialization done.\n");
  delay(1000);

  Serial.println("Begin loop()");
}

void loop()
{
  digitalWrite(LED3, HIGH);
  delay(1000);
  digitalWrite(LED3, LOW);
  delay(1000);
}

void GPIOIsr()
{
  Serial.println("Interrupt Handler");
  if(first)
  {
    Serial.println("Putting MCU to Sleep");
    digitalWrite(LED1, digitalRead(LED1) ^ 1);
    first = 0;
    
    
    Valley_File = SD.open("Testing.txt", FILE_WRITE);
    
    if(Valley_File)
    {
      Valley_File.println("GPIO interrupt");
      Valley_File.close();
    }
    else Serial.println("Valley file error");
    
     GPIOIntClear(GPIO_PORTM_BASE, GPIO_INT_PIN_5);
    //SysCtlSleep(); // Puts the processor into sleep mode
  }
  else
  {
    Serial.println("Wking MCU From Sleep");
    digitalWrite(LED2, digitalRead(LED2) ^ 1);
    first = 1;
    GPIOIntClear(GPIO_PORTM_BASE, GPIO_INT_PIN_5);
  }
  //SysCtlSleep();
}
