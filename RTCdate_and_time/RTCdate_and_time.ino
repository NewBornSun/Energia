#include <Wire.h>

int clockAddress = 0x68;  // This is the I2C address
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}


void setup() {
  Wire.begin();
  Wire.setModule(0); //I2C bus 0
  Serial.begin(115200);
}

void loop() {
   // Reset the register pointer
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

  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.print(second, DEC);
  Serial.print("  ");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.println(year, DEC);
  
  delay(1000); //Print every second
}

