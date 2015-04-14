// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

void GetTimeDate()
{
  Wire.beginTransmission(clockAddress);
  Wire.write(byte(0x00));
  Wire.endTransmission();

  Wire.requestFrom(clockAddress, 7);

  // A few of these need masks because certain bits are control bits
  second     = bcdToDec(Wire.read() & 0x7f);
  minute     = bcdToDec(Wire.read());
  
  // Need to change this if 12 hour am/pm
  hour       = bcdToDec(Wire.read() & 0x3f);  
  dayOfWeek  = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month      = bcdToDec(Wire.read());
  year       = bcdToDec(Wire.read());
  
  //sprintf(linestringtemp1, "%02d-%02d-%02d.csv", month, dayOfMonth, year);
  //int i;
  //for(i = 0; i < sizeof(linestringtemp1) - 1; i ++) Valley_Filename[i] = linestringtemp1[i];
}

void CheckFileName()
{
  if( (file_dayofmonth != dayOfMonth)||(file_month != month)||(file_year != year) )
  {
    file_dayofmonth = dayOfMonth;
    file_month = month;
    file_year = year;

    sprintf(OCV_Filename, "%02i-%02i-%02i.csv", file_year, file_month, file_dayofmonth);
  }
  Serial.print("\nOCV_Filename: ");
  Serial.print(OCV_Filename);
  Serial.println("\n");
  
  OCV_File = SD.open(OCV_Filename, FILE_WRITE);
  if(OCV_File) Serial.println("Opened File");
  else Serial.println("Failed");
}

