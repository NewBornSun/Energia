//TMP36 Pin Variables
int sensorPin = A2; //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures
 
/*
 * setup() - this function runs once when you turn your Arduino on
 * We initialize the serial connection with the computer
 */
 //New LINE
 //another
 //and another
 
 //but one more
void setup()
{
  Serial.begin(115200);  //Start the serial connection with the computer
                       //to view the result open the serial monitor 
  pinMode(sensorPin,INPUT);
}
 
void loop()                     // run over and over again
{
 //getting the voltage reading from the temperature sensor
 double reading = analogRead(sensorPin);
 double voltage = reading*12.02/3044;
 Serial.print(reading);
 Serial.print("  ");
 Serial.println(voltage);

 delay(1000);                                     //waiting a second
}
