/*******************************************************/
// Measure the Temperature in Celsius
/*******************************************************/

double Measure_Temperature()
{ 
  temperature_Celsius = 0;
  
  int temperature_counts = analogRead(TemperatureVoltagePin);
  
  temperature_Celsius = ((temperature_counts*3.3*100/4096)-32)*5/9;
  
  return temperature_Celsius;
}
