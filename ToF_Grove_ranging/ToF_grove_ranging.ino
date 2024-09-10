#include "Seeed_vl53l0x.h"
#include <MegunoLink.h>
#include <CommandHandler.h>
#include <ArduinoTimer.h>
#include <Filter.h>
#include <Streaming.h>
Seeed_vl53l0x VL53L0X;

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SERIAL Serial
#endif
int distance = 0;
float newDistance = 0;
float tearDistance = 0;
float realDistance = 0;
int i = 0;
float m = 0.9393;
float b = -12.383;
int a = 0;
long filterWeight = 40;
ExponentialFilter <long>ADCFilter(filterWeight,0);
float filteredDistance;

void setup() {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    SERIAL.begin(115200);
    Status = VL53L0X.VL53L0X_common_init();
    if (VL53L0X_ERROR_NONE != Status) {
        SERIAL.println("start vl53l0x mesurement failed!");
        VL53L0X.print_pal_error(Status);
        while (1);
    }
    VL53L0X.VL53L0X_continuous_ranging_init();
    if (VL53L0X_ERROR_NONE != Status) {
        SERIAL.println("start vl53l0x mesurement failed!");
        VL53L0X.print_pal_error(Status);
        while (1);
    }
}


void loop() {
VL53L0X_RangingMeasurementData_t RangingMeasurementData;
VL53L0X.PerformContinuousRangingMeasurement(&RangingMeasurementData);
 if (a == 0){
 for (i = 0; i<=50; i++){       //loop to calculate the tear distance by taking 51 readings and summing them
  distance = RangingMeasurementData.RangeMilliMeter;
  realDistance = (distance/m)-b;  //calibration 
  newDistance = realDistance + newDistance;
  distance = 0;
  realDistance = 0;
  Serial.println(i);
  delay(100);
  a=1;
 }
 tearDistance = newDistance/51;  // divide by one more than max i value in for loop, zero the sensor 
Serial.print(tearDistance);
}


delay(100);

Serial.print("UL:");Serial.print(100);Serial.print(",");  // limits for the y axis of serial plotter
Serial.print("LL:");Serial.print(-15);Serial.print(",");
SERIAL.print("distance:");
int distance = RangingMeasurementData.RangeMilliMeter;
realDistance = ((distance/m)-b) - tearDistance;
ADCFilter.Filter(realDistance);                    // exponential filter, behaviour can be changed by changing the filterWeight variable value
filteredDistance = ADCFilter.Current();
SERIAL.print(realDistance); Serial.print(",");
Serial.print("filteredDistance:");Serial.println(filteredDistance);

delay(100);   // controls the frequency of the measurement, affects the accuracy
}


