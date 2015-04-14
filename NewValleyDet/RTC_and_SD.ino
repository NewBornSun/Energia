void CheckFileName()
{
    sprintf(OCV_Filename, "%02i-%02i-%02i.csv", temp->tm_mon, temp->tm_mday, year);
}

void SaveBurstToSD()
{
//    newburstdata = 0;
      
//    Serial.println(millis());
    Valley_File = SD.open(Valley_Filename, FILE_WRITE);
    Serial.println("SaveBurstToSD");
    
    if(Valley_File)
    {
      Valley_File.println("Time,SampleRaw,Status,SampleAVG");
      
      for(int j = 0;j<(VDSamples-20);j++)
      {
        //s_VD_s is stored_VD_samples
        sprintf(linestringtemp, "%i,%i,%i,%i", stored_VD_sampletime[j], s_VD_s[j],stored_VD_status[j],avg_VD_s[j]);  
        Valley_File.println(linestringtemp);
      }  
      Valley_File.close();
    }
    else Serial.println("file error");
}

void SaveSOHtoSD(const int pass)
{
  SOH_File = SD.open("SOH.csv", FILE_WRITE);
  Serial.println("SaveSOHtoSD");
  
  if(SOH_File)
  {
    if(SOHfirst)
    {
      SOH_File.println("Pass/Fail,Month,Day,Year");
      first = 0;
    }
    else
    {
      sprintf(SOHstring, "%1i,%02i,%02i,%02i", pass, temp->tm_mon, temp->tm_mday, year);
      SOH_File.println(SOHstring);
    }
    SOH_File.close();
  }
  else Serial.println("SOH file erroe");
}

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
