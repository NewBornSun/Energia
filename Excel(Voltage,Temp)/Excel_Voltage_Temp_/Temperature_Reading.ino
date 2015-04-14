//const int TemperatureVoltagePin = A2;

int Measure_Temperature()
{
  double temperature_Celsius = 0;
  
  double temperature_counts = analogRead(TemperatureVoltagePin);
  
  temperature_Celsius = ((temperature_counts*3.3*100/4096)-32)*5/9;
  
  return temperature_Celsius;
}
