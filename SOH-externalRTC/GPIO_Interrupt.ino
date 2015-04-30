//******************************************************//
// This interrupt is caused by a low reading on pin 30
//******************************************************//

/*void GPIOIsr()
{
  Serial.println("Interrupt Handler");
  EEPROMProgram((uint32_t *)&eepromWriteData, EEPROM_TEST_ADDRESS, 4);
  
  GPIOIntClear(GPIO_PORTM_BASE, GPIO_INT_PIN_5);
}
