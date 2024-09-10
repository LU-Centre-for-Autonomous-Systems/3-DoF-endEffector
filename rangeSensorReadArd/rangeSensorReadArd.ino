#include <MegunoLink.h>
#include <CommandHandler.h>
#include <ArduinoTimer.h>
#include <Filter.h>


float rawDistance = 0;
float filteredDistance;
int incomingData = 0; //defining the varibale 
int dataLength = 6;  // number of bytes per reading, includes an initial message
byte messageBuffer[6]; 
long int distance = 0;
int tearDistance = 0;
float avgTearDistance;
int tearDistanceNew;
long filterWeight = 12;
ExponentialFilter <long>ADCFilter(filterWeight,0);
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
unsigned long totalTime=32000;
const unsigned long period = 5000;
float drift = 0;
float error;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //begin receiving data from sensor
  //while (Serial.available()) {};//this loop does nothing till there is serial data available from sensor
  for (int i = 0; i<=10; i++ ){
    incomingData = Serial.readBytes(messageBuffer, dataLength);
    tearDistanceNew = messageBuffer[2]*256 + messageBuffer[3];
    tearDistance = tearDistance + tearDistanceNew;
    Serial.println(tearDistance);
    }
avgTearDistance = tearDistance/11;
Serial.println(avgTearDistance);
avgTearDistance = (long int) avgTearDistance;
Serial.println("is the tear Distance");
Serial.end();
Serial.begin(115200);
startMillis=millis();
}


void loop() {
  // put your main code here, to run repeatedly:
  // if (Serial.available()) {
  incomingData = Serial.readBytes(messageBuffer, dataLength);// data stored in messagebuffer, with a data length of 6, already defined 
  rawDistance = (messageBuffer[2]*256 + messageBuffer[3])- avgTearDistance; //data given by sensor company substracted by tear distance
  ADCFilter.Filter(rawDistance);
  currentMillis=millis();
  // while (currentMillis <= totalTime){
    if (currentMillis - startMillis >= period){
    Serial.print("UL:");Serial.print(100);Serial.print(",");
    Serial.print("LL:");Serial.print(-15);Serial.print(",");
    Serial.print("Unfiltered_Data:");Serial.print(rawDistance); Serial.print(",");
    filteredDistance = ADCFilter.Current();
    Serial.print("Filtered_Data:"); Serial.println(filteredDistance);
    startMillis = currentMillis;
    error = abs(filteredDistance);
    drift = drift + error;
    }
 // }
  if (millis()=32001){
  drift = drift/6;
  Serial.print(drift);Serial.println(" is the drift for this distance.");
  //  else if (currentMillis=32001) Serial.print("End of test");
  } 
  // }
}
