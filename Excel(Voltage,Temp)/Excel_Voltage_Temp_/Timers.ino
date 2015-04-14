
///////////////////BEGIN TIMER 0//////////////////////////////////

void initTimer0_OCV()
{  
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);   // 32 bits Timer
  TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0Isr);    // Registering  isr       
  ROM_TimerEnable(TIMER0_BASE, TIMER_A); 
  ROM_IntEnable(INT_TIMER0A); 
  ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  
  
  unsigned long ulPeriod = (120000000 / OCV_Frequency);
  Serial.print("SysCtlClockGet = ");
  Serial.println(SysCtlClockGet());
  Serial.print("u1Period = ");
  Serial.println(ulPeriod);
  ROM_TimerLoadSet(TIMER0_BASE, TIMER_A,ulPeriod -1);
  
}

///////////////////////END Timer 0////////////////////////////////////


///////////////////BEGIN TIMER 1//////////////////////////////////

void initTimer1_Valley()
{  
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);   // 32 bits Timer
  TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1Isr);    // Registering  isr       
  ROM_TimerEnable(TIMER1_BASE, TIMER_A); 
  ROM_IntEnable(INT_TIMER1A); 
  ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);  
  
  unsigned long u2Period = (120000000 / VDFreq);
  Serial.print("SysCtlClockGet = ");
  Serial.println(SysCtlClockGet());
  Serial.print("u2Period = ");
  Serial.println(u2Period);
  ROM_TimerLoadSet(TIMER1_BASE, TIMER_A,u2Period -1);
  
}

///////////////////////END Timer 1////////////////////////////////////

