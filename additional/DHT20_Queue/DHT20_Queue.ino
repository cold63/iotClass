#include <DFRobot_DHT20.h>


extern "C"{
#include "main_queue.h"
}
DFRobot_DHT20 dht20;

extern int front_v;
extern int rear_v;

unsigned long CurrentTime,preTime;
const int intervalSwitch = 1000;
double TempVale,HumidityValue;
int count;


void setup()
{
	Serial.begin(115200);

   //初始化 dht20
   while(dht20.begin()){
      Serial.println("Initialize sensor failed");
      delay(1000);
   }
}

void loop()
{
    logData d;
    d.HumidityValue = dht20.getHumidity();
    delay(1);
    d.TempVale = dht20.getTemperature();
    d.idx = count++;
    enqueue(d);
    

   CurrentTime = millis();
   if(CurrentTime - preTime > intervalSwitch){
		preTime = CurrentTime;

     
     logData val;
     while(!isQueueEmpty())
     {
        dequeue(&val);
        //Serial.print("");
        Serial.print(val.idx);
        Serial.print(": temp:");
        Serial.print(val.TempVale);
        Serial.print(" Humidity:");
        Serial.println(val.HumidityValue * 100);

     }
     

   }

    //delay(300);
    
   
}
