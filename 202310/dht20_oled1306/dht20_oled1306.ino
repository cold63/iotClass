
#include <U8g2lib.h>
#include <DFRobot_DHT20.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

DFRobot_DHT20 dht20;
unsigned long CurrentTime,preTime;
const int intervalSwitch = 1000;
double TempVale,HumidityValue;

int fontWitdh;

void setup()
{
	Serial.begin(115200);

  u8g2.begin();
  u8g2.setFont(u8g2_font_8x13_mf);
  u8g2.setFontPosTop();
  fontWitdh = u8g2.getMaxCharWidth();

   //初始化 dht20
   while(dht20.begin()){
      Serial.println("Initialize sensor failed");
      delay(1000);
   }
   u8g2.clear();
   u8g2.drawStr(0,20,"Temp: ");
   u8g2.drawStr(0,40,"Humidity:");
   u8g2.sendBuffer();

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

      delay(5);

      HumidityValue = dht20.getHumidity() * 100; //讀取濕度
      Serial.print("Humidity: "); //將結果印出來
      Serial.print(HumidityValue);
      Serial.println("%");

      delay(5);
      
     
      u8g2.setCursor(fontWitdh * 5 + 1,20);
     // u8g2.print("      ");
      u8g2.updateDisplay();
      delay(1);
      u8g2.setCursor(fontWitdh * 5 + 1,20);
      u8g2.print(TempVale);
      u8g2.print("C");
      u8g2.updateDisplay();
      delay(1);
      u8g2.setCursor(fontWitdh * 9 + 1,40);
     // u8g2.print("      ");
      u8g2.updateDisplay();   
      delay(1);
      u8g2.setCursor(fontWitdh * 9 + 1,40);
      u8g2.print(HumidityValue);     
      u8g2.updateDisplay();
   }

   
}
