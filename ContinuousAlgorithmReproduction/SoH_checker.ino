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
  
  int OCV_mv = millivoltspercount*OCV;
  int V1_mv = millivoltspercount*Valley1;
  int V2_mv = millivoltspercount*Valley2;
  
  int dV2thresh = multiMap(Temperature,dV2thresh_TempData,dV2thresh_Vth1,9);

  int dV1mod = multiMap((OCV_mv - V1_mv),dV1mod_dVData,dV1mod_Vth2,9);


  int dvthresh_complete = dV2thresh + dV1mod ;
  
  
  char tempstring[50] = "";
  //sprintf(tempstring, "OCV %i,dvthr %i", OCV_mv, dvthresh_complete);
  sprintf(tempstring, "OCV %i,V1 %i,V2 %i, dv2th %i, dv1mod %i,dvthr %i", OCV_mv, V1_mv, V2_mv, dV2thresh, dV1mod, dvthresh_complete);
  Serial.println(tempstring);
    
  int Pass = 0;
  
  SOH_Metric[SOH_Index] = (V2_mv - V1_mv) - dvthresh_complete;
  if(SOH_Index)
  {
    int previous = SOH_Metric[SOH_Index]*SOH_Metric[SOH_Index-1];
    if(previous > 0) warning = 1;
  }
  
  if( (V2_mv - V1_mv) > dvthresh_complete )
  {
    Pass = 1;
  }
  
  double V1 = voltspercount*Valley1;
  double V2 = voltspercount*Valley2;
  double OCV_V = voltspercount*OCV;
  
  SaveSOHtoSD(Pass, V1, V2, OCV_V, Temperature, SOH_Metric[SOH_Index], SoC_recent, warning);
  
   SOH_Index++;
  return Pass;
  
}

