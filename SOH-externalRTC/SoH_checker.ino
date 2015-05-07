/*******************************************************/
// Tables and Variables
/*******************************************************/

//thresholds for recovery voltage at various temperatures
int dV2thresh_TempData[9] = {-30, -20, -10, 0, 10, 20, 30, 40, 50};
//This voltage data has been multiplied by 1000 so it is in millivolts (and an int)
int dV2thresh_Vth1[9] = {-100, 0, 100, 200, 300, 400, 400, 400, 400};

//modifier for initial droop, also in millivolts (1000 = 1V)
int dV1mod_dVData[9] = {1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000};
//small droop will provide a lower failure threshold. The logic being that
//even if it doesn't recover quickly, the small drop still indicates a good
//battery. 
//This data is also now in millivolts
int dV1mod_Vth2[9] = {-160, -125, -90, -55, -20, 15, 50, 85, 120};

int SOH_Index = 0;
int SOH_Metric[10];
int warning = 0;


/******************************************************************/
// Check SOH using Valleys, OCV, and Temperature
/******************************************************************/

int CheckSOH(const int Valley1, const int Valley2, const unsigned int OCV, const double Temperature, const int SoC_recent)
{
  double SoH_Metric_Array[10]; 
  int OCV_mv = millivoltspercount*OCV;
  int V1_mv = millivoltspercount*Valley1;
  int V2_mv = millivoltspercount*Valley2;
  
  OCV_Volts = voltspercount*OCV;
  V1_Volts = voltspercount*Valley1;
  V2_Volts = voltspercount*Valley2;
  deltaV1 = OCV_Volts - V1_Volts;
  deltaV2_Volts = V2_Volts - V1_Volts;
  
  //Serial.println(OCV_Volts);
  //Serial.println(V1_Volts);
  //Serial.println(V2_Volts);
  //Serial.println(deltaV1_volts);
  //Serial.print("deltaV2");
  //Serial.println(deltaV2_Volts);
  
  
  int dV2thresh = multiMap(Temperature,dV2thresh_TempData,dV2thresh_Vth1,9);

  int dV1mod = multiMap((OCV_mv - V1_mv),dV1mod_dVData,dV1mod_Vth2,9);


  int dvthresh_complete = dV2thresh + dV1mod ;
   
  //char tempstring[50] = "";
  //sprintf(tempstring, "OCV %0.2f,V1 %0.2f,V2 %0.2i, deltaV1 %0.2f, VTH1 %0.2f VTH2 %0.2f", OCV_Volts, V1_Volts, V2_Volts, deltaV1_volts, VTH1, VTH2);
  //Serial.println(tempstring);
  
  //****************************/
  // JI Hyun Implementation:
  //****************************/
  Serial.print("Temperature ");
  Serial.print(Temperature);
  
  VTH1 = ((-0.0011*Temperature*Temperature*Temperature) + (0.0005*Temperature*Temperature) + (4.4526*Temperature) + 160)/1000;
  Serial.print("VTH1 ");
  Serial.println(VTH1);
  
  Serial.print("deltaV1 ");
  Serial.println(deltaV1);
  
  double calc = (0.3839*deltaV1*deltaV1*deltaV1*deltaV1*deltaV1*deltaV1) + (-11.044*deltaV1*deltaV1*deltaV1*deltaV1*deltaV1)
  + (128.82*deltaV1*deltaV1*deltaV1*deltaV1) + (-769.14*deltaV1*deltaV1*deltaV1) + (2375.5*deltaV1*deltaV1)
  + (-3000*deltaV1) + 514.62;
  
  VTH2 = calc/1000;
  Serial.print("VTH2 ");
  Serial.println(VTH2);
  
  Serial.print("SoC Max ");
  Serial.println(SoC_Max);
  
  VTH3 = ((-0.0099*SoC_Max*SoC_Max) - (2.1002*SoC_Max) + 298.78)/1000;
  Serial.print("VTH3 ");
  Serial.println(VTH3);
 
  Vth = VTH1 + VTH2 + VTH3;
  Serial.print("Vth ");
  Serial.println(Vth);
 
  Serial.print("deltaV2 " );
  Serial.println(deltaV2_Volts);
  
  SoH_Metric = deltaV2_Volts - Vth;
  SoH_Metric_Array[SOH_Index] = SoH_Metric;
  Serial.print("SoH Metric ");
  Serial.println(SoH_Metric);
 
  if(SoH_Metric > 0) Pass = 1;
  
  if(SOH_Index)
  {
    int previous = SoH_Metric_Array[SOH_Index]*SoH_Metric_Array[SOH_Index-1];
    if(previous > 0) Warning = 1;
    else Warning = 0;
    Serial.println(Warning);
  }
  
  
  //****************************/
  // JI Hyun Implementation:
  //****************************/
  
  
  //sprintf(tempstring, "OCV %i,dvthr %i", OCV_mv, dvthresh_complete);
  
  double V1 = voltspercount*Valley1;
  double V2 = voltspercount*Valley2;
  double OCV_V = voltspercount*OCV;
//  
//  SOH_Metric[SOH_Index] = (V2_mv - V1_mv) - dvthresh_complete;
//  if(SOH_Index)
//  {
//    int previous = SOH_Metric[SOH_Index]*SOH_Metric[SOH_Index-1];
//    if(previous > 0) warning = 1;
//  }
//  
//  if( (V2_mv - V1_mv) > dvthresh_complete )
//  {
//    Pass = 1;
//  }
//  
  // reset parameters
  SoH_check = 1;
  SoCflag = 0;
  ValleyDetect_Flag = 0;
  OCV_Flag = 0;
  SOH_Index++;
  status = initialized;

  return Pass;  
}

