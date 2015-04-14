void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop()
{
  // put your main code here, to run repeatedly:
  int temperature_counts = analogRead(A19);
  float val = temperature_counts*1.821/2304;
  float temperature_Celsius = (10.888-sqrt(118.548544+0.01388*(1777.3-val*1000.0)))/(2*-0.00347)+30.0;
  //Serial.print(temperature_counts);
  Serial.println(temperature_Celsius);
  delay(500);
}
