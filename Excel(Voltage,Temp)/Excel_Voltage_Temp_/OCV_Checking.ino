void Timer0Isr(void) //Timer0 Interrupt Service Routine
{
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Clear the timer interrupt
  digitalWrite(LED1, digitalRead(LED1) ^ 1);              // toggle LED pin
  
  OCV_Measure_Flag = 1;
  count++;
}


const unsigned char OCV_Samples = 10;  //Number of Samples for OCV

unsigned int OCV_meas_index = 0;
unsigned int OCV_samples[OCV_Samples];
unsigned int temperature_samples[OCV_Samples];
unsigned int OCV_SoC_Estimates[OCV_Samples];
unsigned char OCV_valid[OCV_Samples];
unsigned long OCV_sampletime[OCV_Samples];

int OCV_upper_limit = 13*countspervolt;
int OCV_lower_limit = 11*countspervolt;

int OCV_settle_threshold = 0.1*countspervolt;


void OCV_Checker()
{
  //Serial.println("OCV_checker");
  GetTimeDate();
  Serial.println("Finished Getting Time and Date");
  
  
  OCV_samples[OCV_meas_index] = analogRead(TemperatureVoltagePin);
  OCV_sampletime[OCV_meas_index] = millis();
  temperature_samples[OCV_meas_index] = Measure_Temperature();
  
}
