const int tempSensor = A1; //Temp sensor on analog pin A0;

void setup()
{
  Serial.begin(115200);
   pinMode(tempSensor, INPUT); //Classify sensor as input
}
void loop()
{
  float tempReading;               //Store ADC values in this variable
  float conversion = 3.3/4096;  //12-bit conversion number
  float milliVolt, milliVolt1 = 1821;
  float temp1, temp, temp2, test, adc;
  double tempConversion = .010;  //From transfer function equation (10mV)
  double temperature, temperature_Celsius;
  
  tempReading = analogRead(tempSensor); //Take readings
  milliVolt = tempReading*conversion*1000 + 119;     //Convert to voltage
  
  temp1 = 10.888 - sqrt((-10.888)*(-10.888)+(4*0.00357*(1777.3 - milliVolt)));
  temp2 = (temp1/(2*(-0.00347)))+30;
  temp = temp2 + 30;
  
  Serial.print("Millivolt: ");
  Serial.print(milliVolt);
  Serial.print("    Temp: ");
  Serial.println(temp2);
 
  delay(1000);
}
