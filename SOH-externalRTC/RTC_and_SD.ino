byte bcdToDec(byte val)  {// Convert binary coded decimal to normal decimal numbers
  return ( ((val/16)*10) + (val%16) );
}

/*********************************************/
// Get current time and date from RTC
/*********************************************/

void GetTimeDate()  
{  //Fill the time and date variables
    Wire.beginTransmission(0x68);
    Wire.write(byte(0x00));
    Wire.endTransmission();
 
    Wire.requestFrom(0x68, 7);
    int seconds_temp = bcdToDec(Wire.read());
    int minutes_temp = bcdToDec(Wire.read());
    int hours_temp = bcdToDec(Wire.read() & 0b111111); //24 hour time
    int dayofweek_temp = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
    int dayofmonth_temp = bcdToDec(Wire.read());
    int months_temp = bcdToDec(Wire.read());
    int years_temp =  bcdToDec(Wire.read());
    
    if(years_temp == 15)
    {
      seconds = seconds_temp;
      minutes = minutes_temp;
      hours = hours_temp; //24 hour time
      dayofweek = dayofweek_temp; //0-6 -> sunday - Saturday
      dayofmonth = dayofmonth_temp;
      months = months_temp;
      years = years_temp;
    }   
}

/*********************************************/
// Check for new file name
/*********************************************/

void CheckFileName()  //Check if we need a new filename... New file for each day
{
  GetTimeDate();
  
  if( (file_dayofmonth != dayofmonth)||(file_month != months)||(file_year != years) )
  {
    //   Serial.println(OCV_filename);
    file_dayofmonth = dayofmonth;
    file_month = months;
    file_year = years;

    sprintf(OCV_Filename, "%02i-%02i-%02i.csv", file_month, file_dayofmonth, file_year);

    Serial.println(OCV_Filename);
  }
  
}
/*****************************************/
// Save Valley Data to SD
/*****************************************/

void SaveBurstToSD()
{
    Valley_File = SD.open(Valley_Filename, FILE_WRITE);
    //Serial.println("SaveBurstToSD");
    
    if(Valley_File)
    {
      Valley_File.println("Time,SampleRaw,Status,SampleAVG,Bat.Voltage");
      
      for(int j = 0;j<(VDSamples-20);j++)
      {
        //s_VD_s is stored_VD_samples
        float voltage = avg_VD_s[j]*voltspercount;
        if(j <= 2 && avg_VD_s[j] == 0) voltage = avg_VD_s[j+3]*voltspercount;
        sprintf(linestringtemp, "%i,%i,%i,%i,%.4f", stored_VD_sampletime[j], s_VD_s[j],stored_VD_status[j],avg_VD_s[j],voltage);  
        
        Valley_File.println(linestringtemp);
      }  
      Valley_File.close();
    }
    else Serial.println("Valley file error");
}

/*********************************************/
// Save SOH Data to SD
/*********************************************/

void SaveSOHtoSD(const int pass, const double Valley1, const double Valley2, const double OCV, const double Temperature, const int SOH_Metric, int SoC, int warning)
{ 
  //GetTimeDate();
 
  SOH_File = SD.open("SOH.csv", FILE_WRITE);
  Serial.println("SaveSOHtoSD");
  
  if(SOH_File)
  {
    if(SOHfirst)
    {
      SOH_File.println("Pass/Fail,mm/dd/yy,Hour:Min,Valley1,Valley2,OCV,Temperature,SoC, SOH Metric, Warning");
      sprintf(SOHstring, "%1i,%02i/%02i/%02i,%02i:%02i,%.4f,%.4f,%.4f,%.2f,%i,%i,%i", pass, months, dayofmonth, years, hours, minutes, Valley1, Valley2, OCV, Temperature, SoC, SOH_Metric, warning);
      SOH_File.println(SOHstring);
      SOHfirst = 0;
    }
    else
    {
      sprintf(SOHstring, "%1i,%02i/%02i/%02i,%02i:%02i,%.4f,%.4f,%.4f,%.2f,%i,%i,%i", pass, months, dayofmonth, years, hours, minutes, Valley1, Valley2, OCV, Temperature, SoC, SOH_Metric, warning);
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
      OCV_File.println("OCV_samples, measvolts, OCV_SoC_Estimates, temperature_samples, OCV_valid, year, month, dayOfMonth, hour, minute, second, OCV_meas_index");
      OCV_File.println(dataString);
      flag=1;
      OCV_File.close();
    }
    else
    { 
      OCV_File.println(dataString);
      OCV_File.close();
    }
  }  
  // if the file isn't open, pop up an error:
  else Serial.println("OCV file error");
}

