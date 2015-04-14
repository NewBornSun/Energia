/*******************************************************/
// Tables and Variables
/*******************************************************/

//OCV_OCVdata is the x-data, so it has been extended between 0 and 3,
//repeating the y-data

/*  OCV_OCVdata = [0, 1.876, 1.907, 1.932, 1.951, 1.963, 1.976, 1.989, 
    2.004, 2.016, 2.030, 2.045, 2.058, 2.071, 2.083, 2.097, 2.110, 
    2.122, 2.135, 3];  */
//this is OCV data multiplied by 1000 so it could be stored as an int
//Be sure to include trailing zeros
int OCV_OCVdata[20] = {0, 11256, 11442, 11592, 11706, 11778, 11856, 
    11934, 12024, 12096, 12180, 12270, 12348, 12426, 12498, 
    12582, 12660, 12732, 12810, 18000};

//this is OCV_SG data multiplied by 1000 so it could be stored as an int
//Be sure to include trailing zeros
int OCV_SGdata[20] = {1030, 1030, 1051, 1068, 1081, 1089, 1104, 1119, 
    1137, 1152, 1169, 1186, 1202, 1216, 1232, 1248, 1262, 
    1278, 1293, 1293};

//TC_SGdata is the x data, so it has been extended between 0 and 3,
//repeating the y-data. It has been multiplied by 1000 so it could be stored as an int
//Be sure to include trailing zeros
int TC_SGdata[14] = {0, 1025, 1050, 1075, 1100, 1125, 1150, 1175, 1200, 1225, 1250, 1275, 1300, 3000};

//TC_TCdata = [-0.02, -0.02, 0.1, 0.175, 0.24, 0.27, 0.28, 0.28, 0.27, 0.255, 0.23, 0.215, 0.175, 0.175];
//scale TCdata to a 6-cell 12V battery and uV
int TC_TCdata[14] = {-0120, -0120, 060, 1050, 1440, 1620, 1680, 1680, 1620, 1530, 1380, 1290, 1050, 105};

int SoC_SoCdata[5] = {0, 25, 50, 75, 100};
int SoC_SGdata[5] = {1120, 1155, 1190, 1225, 1265};
//variable declarations


int OCV_25C[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float TC_appx[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int SG_appx[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/***********************************************************/
// Estimate_SoC is called if OCV was good in OCV Checker
/***********************************************************/  

char Estimate_SoC(int OCV, int temperature)
{
  float OCV_millivolts = OCV*millivoltspercount;
  /*
  Serial.print("OCV = ");
  Serial.println(OCV_millivolts);
  Serial.print("temperature = ");
  Serial.println(temperature);
  */
  char SoC = 0;
  //Serial.println("Estimate_SoC");
  int SpecificGravity = Estimate_SG(OCV_millivolts, temperature);
  
  //    Serial.print("SpecificGravity = ");
 //     Serial.println(SpecificGravity);
      
  SoC = multiMap(SpecificGravity,SoC_SGdata,SoC_SoCdata,5);
  
  return SoC;
}

/***********************************************************/
// Estimate Specific Gravity based on OCV and Temp
/***********************************************************/  

int Estimate_SG(int OCV, int temperature)
{
  int SpecificGravityInternal = 0;
  
  //This for loop determines what the OCV would be, if the battery was at 25C
  //First, it uses the OCV to find SG and a temperature coefficient
  //then it uses that temp co to estimate what the OCV would be at 25C
  //The new OCV estimate is used on the next iteration to find a more accurate SG and temp co.
  //After 10 iterations, the values are very stagnant
  for(int m = 1; m < 10; m++)
  {
    //Look up SG using OCV
    SG_appx[m] = multiMap(OCV_25C[m-1],OCV_OCVdata,OCV_SGdata,20);
    
    //Look up Temp Coefficient using SG
    TC_appx[m] = multiMap(SG_appx[m],TC_SGdata,TC_TCdata,14) / 1000;
    
    //Calculate the 25C OCV using the original OCV, temperature, and new temp co
    OCV_25C[m] = OCV - (25-temperature)*TC_appx[m];
  }
  
  SpecificGravityInternal = multiMap(OCV_25C[9],OCV_OCVdata,OCV_SGdata,20);

  
  return SpecificGravityInternal;
}

/***********************************************************/
// Mapping Function
// http://playground.arduino.cc/Main/MultiMap
// note: the _in array should have increasing values
/***********************************************************/  

int multiMap(int val, int* _in, int* _out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];

  // search right interval
  uint8_t pos = 1;  // _in[0] allready tested
  while(val > _in[pos]) pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos]) return _out[pos];

  // interpolate in the right segment for the rest
  return map(val, _in[pos-1], _in[pos], _out[pos-1], _out[pos]);
}
