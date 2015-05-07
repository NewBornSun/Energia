/*******************************************************/
// Variables for storing data
/*******************************************************/

const unsigned char OCV_Samples = 3;  //Number of Samples for OCV

unsigned int OCV_meas_index = 0;
unsigned int OCV_samples[OCV_Samples];
double temperature_samples[OCV_Samples];
unsigned int OCV_SoC_Estimates[OCV_Samples];
unsigned char OCV_valid[OCV_Samples];
unsigned long OCV_sampletime[OCV_Samples];
//unsigned int SoC_Max = 0;

int OCV_upper_limit = 13*countspervolt;
int OCV_lower_limit = 11*countspervolt;

int OCV_settle_threshold = 0.1*countspervolt;

/*******************************************************/
// Timer0 interrupt service routine
/*******************************************************/

void Timer0Isr(void) //Timer0 Interrupt Service Routine
{
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Clear the timer interrupt
  digitalWrite(LED1, digitalRead(LED1) ^ 1);              // toggle LED pin
  
  OCV_Measure_Flag = 1;
}


/*******************************************************/
// OCV_Checker()
/*******************************************************/

void OCV_Checker()
{
  //EEPROMRead((uint32_t *)&eepromReadData, EEPROM_TEST_ADDRESS, 4); // Read Contents
  
  //if(eepromReadData == 150) 
  //{ 
     //digitalWrite(LED4, HIGH);
     //EEPROMProgram((uint32_t *)&restoreEEPROM, EEPROM_TEST_ADDRESS, 4); // Restore Contents
     //SaveSOHtoSD(0, 0, 0, 0, 0, -10, 0, 0);
  //}
  GetTimeDate();
  
  OCV_samples[OCV_meas_index] = analogRead(BatteryVoltagePin);
  OCV_sampletime[OCV_meas_index] = millis();
  temperature_samples[OCV_meas_index] = Measure_Temperature();
  
  CheckMinMaxOCV();

  MakeDataString();
  SaveDataString(dataString); //dataString
  Serial.println(serialData);
  OCV_Measure_Flag = 0;
  
  //this causes the index to point to the value that will be replaced next
  OCV_meas_index++;
  OCV_meas_index = OCV_meas_index%OCV_Samples;
}


/*******************************************************/
// Check for Min and Max OCV values
/*******************************************************/

void CheckMinMaxOCV()
{
  
  OCV_SoC_Estimates[OCV_meas_index] = 0;
  digitalWrite(LED2, 0);              // LED off
  if( (OCV_samples[OCV_meas_index] < OCV_upper_limit) && (OCV_samples[OCV_meas_index] > OCV_lower_limit) )
  {
  //if the most recent value is valid... check them all?
    if( (maxOCV() - minOCV()) < OCV_settle_threshold )
    {
      //if the difference between min and max is okay, say it's valid
      OCV_valid[OCV_meas_index] = 1;
      OCV_SoC_Estimates[OCV_meas_index] = Estimate_SoC( OCV_samples[OCV_meas_index] , temperature_samples[OCV_meas_index]);
      
      if(OCV_SoC_Estimates[OCV_meas_index] > SoC_Max) SoC_Max = OCV_SoC_Estimates[OCV_meas_index];
      
      OCV_Flag = 1;
      if(SoC_Max > 50)
      { 
        digitalWrite(LED2, 1);
        SoCflag = 1;
      }
      else SoCflag = 0;
 
      //Serial.print("OCV_SoC_Estimates = ");
      //Serial.println(OCV_SoC_Estimates[OCV_meas_index]);
      digitalWrite(LED2, 1);              // LED on
      
    }
    else OCV_valid[OCV_meas_index] = 0; //if not, then it is invalid
  }
  else OCV_valid[OCV_meas_index] = 0; //if most recent value is invalid, OCV has now expired
}
  
/*******************************************************/
// Make data string for storing onto SD card
/*******************************************************/

void MakeDataString() 
{
  //GetTimeDate(); 
  
  measvolts = OCV_samples[OCV_meas_index]*voltspercount;
  sprintf(dataString, "%i,%.4f,%i,%.2f,%u,%i,%i,%i,%i,%i,%i,%i", OCV_samples[OCV_meas_index], measvolts, 
  OCV_SoC_Estimates[OCV_meas_index], temperature_samples[OCV_meas_index], OCV_valid[OCV_meas_index], 
  years, months, dayofmonth, hours, minutes, seconds, OCV_meas_index);
  
  sprintf(serialData, "Bat Voltage: %i Baterry Voltage: %.4f Temperature: %.2f  OCV Valid: %i SoC Estimate: %i  SoC Max: %i",OCV_samples[OCV_meas_index],
  measvolts, temperature_samples[OCV_meas_index], OCV_valid[OCV_meas_index], OCV_SoC_Estimates[OCV_meas_index], SoC_Max);
}

/*******************************************************/
// maxOCV() and minOCV()
/*******************************************************/

int maxOCV()
{
  int maximum = 0;
  for (int l = 0; l < OCV_Samples; l++)
  {
    if(OCV_samples[l] > maximum) maximum = OCV_samples[l];
  }
  return maximum;
}

int minOCV()
{
  int minimum = 0xffff;
  for (int l = 0; l < OCV_Samples; l++)
  {
    if(OCV_samples[l] < minimum) minimum = OCV_samples[l];
  }
  return minimum;
}

/*******************************************************/
// Find recent SoC Reference
/*******************************************************/

unsigned int FindRecentSoCRef()
{
  SoC_recent = 0xffff;
  unsigned int OCV_recent = 0xffff;
  unsigned int OCV_ref = 0xffff;
  
  for (unsigned int l = OCV_meas_index; l != (OCV_meas_index+OCV_Samples-1)%OCV_Samples; l = (l+1)%OCV_Samples)
  {
    /*
    Serial.print("(OCV_meas_index+OCV_Samples-1)%OCV_Samples = ");
    Serial.print((OCV_meas_index+OCV_Samples-1)%OCV_Samples);
    Serial.print(" FindRecentSoC l = ");
    Serial.print(l);
    Serial.print(" OCV_valid[l] = ");
    Serial.println(OCV_valid[l]);
    */
    
    if(OCV_valid[l])
    {
      SoC_recent = OCV_SoC_Estimates[l];
      OCV_recent = OCV_samples[l];
      OCV_ref = l;
    }
    else
    {
      //SoC_recent = 0xffff;
    }
  }
  return OCV_ref;
  //return SoC_Max;
}

/*******************************************************/
// Lookup Functions
/*******************************************************/

unsigned int lookupOCV(unsigned int ref)
{
  if(ref < OCV_Samples)  return OCV_samples[ref];
  else return 0xffff;
}

unsigned int lookupSoC(unsigned int ref)
{
  if(ref < OCV_Samples)  return SoC_Max; //OCV_SoC_Estimates[ref];
  else return 0xffff;
}

double lookupTemp(unsigned int ref)
{
  if(ref < OCV_Samples)  return temperature_samples[ref];
  else return 0xffff;
}
