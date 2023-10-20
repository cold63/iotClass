
#include <DFRobot_DHT20.h>

DFRobot_DHT20 dht20;

unsigned long CurrentTime,preTime;
const int intervalSwitch = 1000;
double TempVale,HumidityValue;

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
   CurrentTime = millis();
   if(CurrentTime - preTime > intervalSwitch){
		preTime = CurrentTime;

      TempVale = dht20.getTemperature();  //讀取溫度
      Serial.print("Temp: ");    //將結果印出來
      Serial.print(TempVale);
      Serial.println("C");

      delay(1);

      HumidityValue = dht20.getHumidity() * 100; //讀取濕度
      Serial.print("Humidity: "); //將結果印出來
      Serial.print(HumidityValue);
      Serial.println("%");

   }

   
}
