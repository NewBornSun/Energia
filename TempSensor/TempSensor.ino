const int tempSensor = A3; //Temp sensor on analog pin A0;

void setup()
{
  Serial.begin(115200);
   pinMode(tempSensor, INPUT); //Classify sensor as input
}
void loop()
{ 
  int tempReading;               //Store ADC values in this variable
  float conversion = 3.3/4096;  //12-bit conversion number
  double voltage;
  double tempConversion = 0.010;  //From transfer function equation (10mV)
  double temperature, temperature_Celsius;
  
  tempReading = analogRead(tempSensor); //Take readings
  voltage = tempReading*conversion;     //Convert to voltage
  temperature = voltage/tempConversion; //Convert to temperature (Fahrenheit)
  temperature_Celsius = ((tempReading*3.3*100/4096)-32)*5/9;
  
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print("    Celsius: ");
  Serial.print(temperature_Celsius);
  Serial.print("     Farehnheit: ");
  Serial.println(temperature);  //Print on Serial Monitor
  delay(1000);
}
