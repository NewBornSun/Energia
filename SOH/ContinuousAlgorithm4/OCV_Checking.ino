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

//called by Timer0ISR...
void Timer0Isr(void)
{
  ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Clear the timer interrupt
  digitalWrite(LED1, digitalRead(LED1) ^ 1);              // toggle LED pin
  
  OCV_Measure_Flag = 1;
}

//called by loop if OCV_Measure_Flag is high
void OCV_checker()
{
  //Serial.println("OCV_checker");
  GetTimeDate();
  
  
  OCV_samples[OCV_meas_index] = analogRead(BatteryVoltagePin);
  OCV_sampletime[OCV_meas_index] = millis();
  temperature_samples[OCV_meas_index] = Measure_Temperature();
  
  CheckMinMaxOCV();

  MakeDataString();
  SaveDataString();
  Serial.println(dataString);
  OCV_Measure_Flag = 0;
  
  //this causes the index to point to the value that will be replaced next
  OCV_meas_index++;
  OCV_meas_index = OCV_meas_index%OCV_Samples;
}



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
      
      
      Serial.print("OCV_SoC_Estimates = ");
      Serial.println(OCV_SoC_Estimates[OCV_meas_index]);
      digitalWrite(LED2, 1);              // LED on
      
    }
    else
    {
      //if not, then it is invalid
      OCV_valid[OCV_meas_index] = 0;
    } 
  }
  else
  {
    //if most recent value is invalid, OCV has now expired
    OCV_valid[OCV_meas_index] = 0;
  }
}
  
//called by OCV_checker (in this file)
void MakeDataString() 
{
  measvolts = OCV_samples[OCV_meas_index]*voltspercount;
  sprintf(dataString, "%i,%.4f,%i,%i,%u,%i,%i,%i,%i,%i,%i", OCV_samples[OCV_meas_index], measvolts, 
  OCV_SoC_Estimates[OCV_meas_index], temperature_samples[OCV_meas_index], OCV_valid[OCV_meas_index], 
  years, months, dayofmonth, hours, minutes, seconds);
}

//called by OCV_checker (in this file)
void SaveDataString() {
    OCV_File = SD.open(OCV_filename, FILE_WRITE);
    
    if (OCV_File) {
      OCV_File.println(dataString);
      OCV_File.close();
      }  
    // if the file isn't open, pop up an error:
    else {
      Serial.println("file error");
      }
}


int maxOCV()
{
  int maximum = 0;
  for (int l = 0; l < OCV_Samples; l++)
  {
    if(OCV_samples[l] > maximum)
    {
      maximum = OCV_samples[l];
    }
  }
  return maximum;
}

int minOCV()
{
  int minimum = 0xffff;
  for (int l = 0; l < OCV_Samples; l++)
  {
    if(OCV_samples[l] < minimum)
    {
      minimum = OCV_samples[l];
    }
  }
  return minimum;
}

unsigned int FindRecentSoCRef()
{
  unsigned int SoC_recent = 0xffff;
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
}

unsigned int lookupOCV(unsigned int ref)
{
  if(ref < OCV_Samples)  return OCV_samples[ref];
  else return 0xffff;
}

unsigned int lookupSoC(unsigned int ref)
{
  if(ref < OCV_Samples)  return OCV_SoC_Estimates[ref];
  else return 0xffff;
}

unsigned int lookupTemp(unsigned int ref)
{
  if(ref < OCV_Samples)  return temperature_samples[ref];
  else return 0xffff;
}

