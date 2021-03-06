void CheckFileName()
{
    sprintf(OCV_Filename, "%02i-%02i-%02i.csv", temp->tm_mon, temp->tm_mday, year);
}

/*****************************************/
// Save Valley Data to SD
/*****************************************/

void SaveBurstToSD()
{
    Valley_File = SD.open(Valley_Filename, FILE_WRITE);
    Serial.println("SaveBurstToSD");
    
    if(Valley_File)
    {
      Valley_File.println("Time,SampleRaw,Status,SampleAVG,Bat.Voltage");
      
      for(int j = 0;j<(VDSamples-20);j++)
      {
        //s_VD_s is stored_VD_samples
        float voltage = avg_VD_s[j]*voltspercount;
        sprintf(linestringtemp, "%i,%i,%i,%i,%.4f", stored_VD_sampletime[j], s_VD_s[j],avg_VD_s[j],stored_VD_status[j],voltage);  
        Valley_File.println(linestringtemp);
      }  
      Valley_File.close();
    }
    else Serial.println("file error");
}

/*********************************************/
// Save SOH Data to SD
/*********************************************/

void SaveSOHtoSD(const int pass, const int Valley1, const int Valley2, const unsigned int OCV, const int Temperature, const int SOH_Metric)
{
  HibernateCalendarGet(temp);
    
  SOH_File = SD.open("SOH.csv", FILE_WRITE);
  Serial.println("SaveSOHtoSD");
  
  if(SOH_File)
  {
    if(SOHfirst)
    {
      SOH_File.println("Pass/Fail,mm/dd/yy,Hour:Min,Valley1,Valley2,OCV,Temperature,SOH_Metric");
      sprintf(SOHstring, "%1i,%02i/%02i/%02i,%02i:%02i,%i,%i,%i,%i,%i", pass, temp->tm_mon, temp->tm_mday, year, temp->tm_hour, temp->tm_min, Valley1, Valley2, OCV, Temperature, SOH_Metric);
      SOH_File.println(SOHstring);
      first = 0;
    }
    else
    {
      sprintf(SOHstring, "%1i,%02i/%02i/%02i,%02i:%02i,%i,%i,%i,%i,%i", pass, temp->tm_mon, temp->tm_mday, year, temp->tm_hour, temp->tm_min, Valley1, Valley2, OCV, Temperature, SOH_Metric);
      SOH_File.println(SOHstring);
    }
    SOH_File.close();
  }
  else Serial.println("SOH file error");
}

/*********************************************/
// Save OCV Data to SD
/*********************************************/

void SaveDataString(char *dataString) 
{
  OCV_File = SD.open(OCV_Filename, FILE_WRITE);

  if (OCV_File) 
  {
    if(!flag)
    {
      Serial.println("Writing first line");
      OCV_File.println("OCV_samples, measvolts, OCV_SoC_Estimates, temperature_samples, OCV_valid, year, month, dayOfMonth, hour, minute, second, OCV_meas_index");
      flag=1;
      OCV_File.close();
    }
    else
    { 
      Serial.print("Opened OCV_File, writing: ");
      OCV_File.println(dataString);
      OCV_File.close();
    }
  }  
  // if the file isn't open, pop up an error:
  else Serial.println("file error");
}

/*********************************************/
// RTC Interrupt Handler
/*********************************************/

void HibernateHandler(void)
{
  //Use this to reset interrupt flag
  uint32_t ui32Status = HibernateIntStatus(1);
  HibernateIntClear(ui32Status);
  
  //To keep the interrupt hapening every second you need this
  HibernateRTCMatchSet(0,HibernateRTCGet()+1);  
}
