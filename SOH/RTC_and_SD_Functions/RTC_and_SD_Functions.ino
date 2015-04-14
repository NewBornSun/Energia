
byte bcdToDec(byte val)  {// Convert binary coded decimal to normal decimal numbers
  return ( ((val/16)*10) + (val%16) );
}

void GetTimeDate()  {  //Fill the time and date variables
  Wire.beginTransmission(0x68);
    Wire.write(zero);
    Wire.endTransmission();
 
    Wire.requestFrom(0x68, 7);
    int seconds_temp = bcdToDec(Wire.read());
    int minutes_temp = bcdToDec(Wire.read());
    int hours_temp = bcdToDec(Wire.read() & 0b111111); //24 hour time
    int dayofweek_temp = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
    int dayofmonth_temp = bcdToDec(Wire.read());
    int months_temp = bcdToDec(Wire.read());
    int years_temp =  bcdToDec(Wire.read());
    
    if(years_temp == 14)
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

void CheckFileName()  //Check if we need a new filename... New file for each day
{
  if( (file_dayofmonth != dayofmonth)||(file_month != months)||(file_year != years) )
  {
 //   Serial.println(OCV_filename);
    file_dayofmonth = dayofmonth;
    file_month = months;
    file_year = years;

    sprintf(OCV_filename, "%02i-%02i-%02i.csv", file_year, file_month, file_dayofmonth);

    Serial.println(OCV_filename);
  }
  
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
      
        
 //     Serial.println(linestringtemp);
        
      Valley_File.close();
    }
    else 
    {
      Serial.println("file error");
    }
  
}
