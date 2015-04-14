

#include <SPI.h>
#include <SD.h>

Sd2Card card;
SdVolume volume;
File OCV_File;

void setup()
{
  Serial.begin(115200);

  SPI.setModule(2);
  pinMode(3, OUTPUT); 
  
 if (!SD.begin(39)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
 }
 
    
    OCV_File = SD.open("test.csv", FILE_WRITE);

  // if the file opened okay, write to it:
  if (OCV_File) {
    Serial.print("Writing to test.csv...");
    OCV_File.println("testing, 1, 2, 3."); //Comma separated values
    // close the file:
    OCV_File.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.csv");
  }
  
   // re-open the file for reading:
  OCV_File = SD.open("test.csv");
  if (OCV_File) {
    Serial.println("test.csv:");

    // read from the file until there's nothing else in it:
    while (OCV_File.available()) {
      Serial.write(OCV_File.read());
    }
    // close the file:
    OCV_File.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.csv");
  }

  
    
   
}

void loop()
{
  // put your main code here, to run repeatedly:
  
}
