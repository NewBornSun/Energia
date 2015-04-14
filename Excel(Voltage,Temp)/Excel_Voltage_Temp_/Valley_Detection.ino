void Timer1Isr(void)
{
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Clear the timer interrupt
  digitalWrite(LED2, digitalRead(LED1) ^ 1);              // toggle LED pin
  
  //OCV_Measure_Flag = 1;
}
