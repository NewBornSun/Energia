#include "inc/tm4c1294ncpdt.h"
#include "inc/tm4c129xnczad.h"
#include "driverlib/hibernate.c"

/*

  Code made with energia-0101E0012

  This code is suposed to help clarify anyone with doubts of how to use very basic fuctions of the RTC
  in the hibernation peripheral and also the hardware calendar mode. Any sugestions and improvements are always welcome
  
  This example only changes and show hour, minutes and seconds but there's:
    psTime->tm_min
    psTime->tm_sec 
    psTime->tm_mon 
    psTime->tm_mday 
    psTime->tm_wday 
    psTime->tm_year
    psTime->tm_hour

*/

int year;

void HibernateHandler(void)
{
  //Use this to reset interrupt flag
  uint32_t ui32Status = HibernateIntStatus(1);
  HibernateIntClear(ui32Status);
  
  //Place here code to execute every second, ex: LCD or 7 segment display
  //Altough it should be as fastest as possible
  
  
  //To keep the interrupt hapening every second you need this
  HibernateRTCMatchSet(0,HibernateRTCGet()+1);  
}

/*It's need a struct pointer of the type "tm" so i use new to do that. This type of struct is defined in the driverlib/hibernation
  you could also create it like this: tm temp; and then use &temp and temp.values in the fuctions                              
*/
tm *temp = new tm;



void setup()
{
  Serial.begin(115200);
  // put your setup code here, to run once:
  
  //Enable Hibernate peripheral. I'm using Energia so i use F_CPU for geting the clock frequency
  HibernateEnableExpClk(F_CPU);
  HibernateRTCEnable();
  
  //We set a interrupt for the RTC after second. You can change the value
  HibernateRTCMatchSet(0,HibernateRTCGet()+1);
  HibernateIntRegister(HibernateHandler);
  HibernateIntEnable(HIBERNATE_INT_RTC_MATCH_0);
  
  //Set up calender mode. HibernateCounterMode() is always needed but can be set to 12hr mode
  HibernateCounterMode(HIBERNATE_COUNTER_24HR);
  HibernateCalendarSet(temp); //<-- the struct declared
  
  //We change the hour, minutes and seconds
  temp->tm_hour= 13;
  temp->tm_min=10;
  temp->tm_sec = 50;
  temp->tm_mon = 03;
  temp->tm_mday = 21;
  temp->tm_year = 15;
  //This fuction below is what actualy updates the values inside the peripheral. 
  //if you don't use it, the value changes above won't do anytigh
  HibernateCalendarSet(temp);
  
}

void loop()
{
  
  //This is to take the "live" values that the RTC keeps updating into our struct
  HibernateCalendarGet(temp);
  
 
  Serial.print(temp->tm_hour);
   Serial.print(':'); 
   Serial.print(temp->tm_min);
  Serial.print(':'); 
  Serial.println(temp->tm_sec);
  
  Serial.print("mm/dd/yy ");
  Serial.print(temp->tm_mon);
  Serial.print('/');
  Serial.print(temp->tm_mday);
  Serial.print('/');
  year = temp->tm_year;
  Serial.println(year);
  
  delay(1000);
}
