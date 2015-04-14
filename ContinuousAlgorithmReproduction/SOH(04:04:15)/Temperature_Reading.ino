/*******************************************************/
// Measure the Temperature in Celsius
/*******************************************************/

int Measure_Temperature()
{ 
  /*int temperature_counts = analogRead(TemperatureVoltagePin);
  float val = temperature_counts*1.821/2304;
  float temperature_Celsius = (10.888-sqrt(118.548544+0.01388*(1777.3-val*1000.0)))/(2*-0.00347)+30.0;*/
  
  char temperature_Celsius = 0;
  
  int temperature_counts = analogRead(TemperatureVoltagePin);
  
  temperature_Celsius = ((temperature_counts*3.3*100/4096)-32)*5/9;
  
  return temperature_Celsius;
}
