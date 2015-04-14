
/////////////////////Variables for Data Storage
const unsigned int VDSamples = 2000;  //Number of Samples for ValleyDetector


int VD_samples[VDSamples];
int s_VD_s[VDSamples];
int avg_VD_s[VDSamples];
unsigned long VD_sampletime[VDSamples];
unsigned long stored_VD_sampletime[VDSamples];
unsigned long stored_VD_status[VDSamples];

unsigned int fastindex = 0;
unsigned int last_fastindex_start = 0;

//deltaV threshold to recognize cranking
const int dvThresh = 0.25*countspervolt;

//valley voltage filters
const int ValFil = 1;  //0.0025*countspervolt;

//voltage for car to be 'on'
const unsigned int onThresh = 13*countspervolt;


int ValleyDiff = 0;
int Valley2 = 0;
int Valley1 = 0;

int minpos = 0;
int maxpos = 0;
int mn = 4096;
int mx = 0;
int lookformax = 0;
int delta = 5;

void Timer1Isr(void)
{
  ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);  // Clear the timer interrupt
  digitalWrite(LED2, digitalRead(LED1) ^ 1);           // toggle LED pin 
    
  VD_sampletime[fastindex] = micros();
  VD_samples[fastindex] = analogRead(BatteryVoltagePin);
  
  fastindex++;
  
  if( (fastindex == VDSamples) )  //every 10 seconds
  {
    HibernateCalendarGet(temp); //Get values from internal RTC
    
    sprintf(Valley_Filename, "%02i_%02i_%02i.csv", temp->tm_hour, temp->tm_min, temp->tm_sec);
    ValleyDetect_Flag = 1;
    fastindex = 0;
  }
  else if(fastindex == VDSamples/2)  //Happens once every 5 seconds
  {
    HibernateCalendarGet(temp); //Get values from internal RTC
    
    sprintf(Valley_Filename, "%02i_%02i_%02i.csv", temp->tm_hour, temp->tm_min, temp->tm_sec);
    ValleyDetect_Flag = 2;
  }
  else
  {
    
  }  
}

unsigned char Valley_Processor(char VD_Flag)
{
  int startpoint = 0;
  int endpoint = 0;
  unsigned char status = initialized;
  /*Status definitions
  #define initialized 0
  #define VoltSettled 1
  #define dvdtdetected 2  //ready for valley 1
  #define valley1detected 3  //ready for valley 2
  #define valley2detected 4  //ready for onThresh
  #define onThreshpassed 5
  #define SoHfailed 6
  #define SoHpassed 7
  */
 
  startpoint = (fastindex + 10)%VDSamples;
  endpoint = (fastindex - 10)%VDSamples;
  
  
  for(unsigned int n = 0; n < (VDSamples - 20); n++)
  {
    //copy fast measurements to temporary array to allow more time for processing
    unsigned int orig_point = (n + startpoint)%VDSamples;
    
    s_VD_s[n] = VD_samples[orig_point];
    stored_VD_sampletime[n] = VD_sampletime[orig_point];
  }
  
  
  int runningavg = s_VD_s[0] + s_VD_s[1] + s_VD_s[2];
  
  //avg array is even shorter than 'stored' array
  for(unsigned int n = 3; n < (VDSamples - 20); n++)
  {
    
    runningavg = runningavg + (s_VD_s[n]);
    
    avg_VD_s[n] = runningavg >> 2;
    runningavg = runningavg - s_VD_s[n-3];
  }
  
//  Serial.println("1");
  //check for voltagesettled, and set status. If unsettled, return nothing
  unsigned int SoC_recent_ref = FindRecentSoCRef();
  
//  Serial.println(SoC_recent_ref);
  unsigned int OCV_recent = lookupOCV(SoC_recent_ref);
//  Serial.println(OCV_recent);
  unsigned int SoC_recent = lookupSoC(SoC_recent_ref);
  
  int Temp_recent = lookupTemp(SoC_recent_ref);
  
  Serial.print("SoC_recent = ");
  Serial.println(SoC_recent);
  if(SoC_recent < 0xffff)
  {
    status = VoltSettled;
  }
  else
  {
    return status;
  }
 // Serial.println("Voltage Settled");
  
  //UNCOMMENT THIS FOR DEBUGGING, this gets rid of the dv check, so spinning the pot works
  //   status = dvdtdetected;
  
 
  digitalWrite(LED3, 0);              // LED off
  digitalWrite(LED4, 0);              // LED off
  
  //Start chugging through temporary array
  //THIS IS WHERE THE DETECTION HAPPENS
  //bounds are because 0th point is already checked when n == 1 and...
  //the last two points are checked by the 3rd to last n
  for(unsigned int n = 1; n < (VDSamples - 20-2); n++)
  {
    if(status == VoltSettled)
    {
      //If voltage is settled, start checking for an initial drop
      //if voltage drop between any two samples exceeds the threshold get ready for valley 1
      //In the future, we may have to check multiple points for droop
      if( (avg_VD_s[n-1]-avg_VD_s[n]) > dvThresh) 
      {
        status = dvdtdetected;
        Serial.print("dvdtdetected = ");
        //Serial.println((avg_VD_s[n-1]-avg_VD_s[n]));
      }
    }
    else if( (status == dvdtdetected) || (status == valley1detected) )
    {
      if(avg_VD_s[n] > mx){
        mx = avg_VD_s[n];
        maxpos = n;}
        
      if(avg_VD_s[n] < mn){
        mn = avg_VD_s[n];
        minpos = n;}
      
      if(lookformax){
        if(avg_VD_s[n] <  (mx-delta)){
          mn = avg_VD_s[n];
          minpos = n;
          lookformax = 0; }}
          
      else
      {
        if(avg_VD_s[n] > (mn+delta))
        {
          if(status == dvdtdetected)
          {
            Valley1 = avg_VD_s[n];
            mx = avg_VD_s[n];
            maxpos = n;
            status = valley1detected;
            lookformax = 1;
            Serial.print("Valley 1 = ");
            Serial.println(Valley1);
          }
          else if(status == valley1detected)
          {
            Valley2 = avg_VD_s[n];
            mx = avg_VD_s[n];
            maxpos = n;
            status = valley2detected;
            lookformax = 1;
            Serial.print("Valley 2 = ");
            Serial.println(Valley2);
          }
        }
      }
    }
    else if(status == valley2detected)
    {
      //use n+2 so that the last recorded point can be checked
      if( avg_VD_s[n+2] > onThresh )
      {
        ValleyDiff = Valley2-Valley1;
        status = onThreshpassed;
        
   //     Serial.print("onThreshpassed, ValleyDiff = ");
    //    Serial.println(ValleyDiff);
        
        digitalWrite(LED3, 1);              // LED on
        if(CheckSOH(Valley1,Valley2,OCV_recent,Temp_recent) == 1)
        {
          Serial.println("passed");
          status = SoHpassed;
        digitalWrite(LED4, 1);              // LED on
        }
        else
        {
          Serial.println("failed");
          status = SoHfailed;
        }
      }
    }
    
  stored_VD_status[n] = status;
  }
  
  Serial.print("Status");
  Serial.println(stored_VD_status[n]);
  
  return status;
}
