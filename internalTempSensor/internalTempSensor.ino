uint32_t val; 
float temp;  

void setup() {
  Serial.begin(115200);
}

void loop() {
  delay(1000); 
  val = analogRead(TEMPSENSOR); 
  Serial.print("adc:");         //raw ADC data
  Serial.print(val);
  temp = 147.5 - 75*3.3*val/4096;    //temperature in Celsius
  Serial.print("  temp:");      
  Serial.println(temp,1); 
}
