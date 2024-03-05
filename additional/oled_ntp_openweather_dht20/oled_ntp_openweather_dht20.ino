#include <U8g2lib.h>
#include <DFRobot_DHT20.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

int status = WL_IDLE_STATUS;
char ssid[] = "your_ssid";
char pass[] = "your_passwd";

//********** Time ********************************************
char timeServer[] = "time.stdtime.gov.tw";
const int timeZone = 8;

WiFiUDP udp;
unsigned int localPort = 2390;

const int NTP_PACKET_SIZE = 48;
byte packetbuffer[NTP_PACKET_SIZE];
/////Time End

//************************ open weather map **************************
String apikey ="cdcb515cb3ea544e3d133b20ee47e890";
String location ="New Taipei City,TW";
char openWeatherServer[] = "api.openweathermap.org";

JsonDocument doc;
String minTemp,maxTemp,TempValue,humidityStr,weatherStr;
int weatherType;

enum wType{
    SUN,
    SUN_CLOUD,
    CLOUD,
    RAIN,
    THUNDER
};
/////open weather end
WiFiClient client;
unsigned long ConnectTimeOut;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);
int fontWitdh;
int fontHigh;

DFRobot_DHT20 dht20;
unsigned long CurrentTime,preTime;
const int intervalSwitch = 1000;

unsigned long reTryConnTime,reTryConnPreTime;
const int reTryConnIntervalSwitch = 1000 * 5;

double TempVale,HumidityValue;


/*NTP code*/
void sendNTPpacket()
{
    memset(packetbuffer,0,NTP_PACKET_SIZE);

    packetbuffer[0] = 0b11100011;
    packetbuffer[1] = 0;
    packetbuffer[2] = 6;
    packetbuffer[3] = 0xEC;

    packetbuffer[12] = 49;
    packetbuffer[13] = 0x4E;
    packetbuffer[14] = 49;
    packetbuffer[15] = 52;

    udp.beginPacket(timeServer,123);
    udp.write(packetbuffer,NTP_PACKET_SIZE);
    udp.endPacket();
}

time_t getNtpTime()
{
    Serial.println("Transmit NTP Request");
    udp.setRecvTimeout(1500);
    sendNTPpacket();
    if (udp.read(packetbuffer,NTP_PACKET_SIZE) > 0){
        unsigned long secsSinec1900;
        //convert four bytes starting at location 40 to a long integer
        secsSinec1900 = (unsigned long)packetbuffer[40] << 24;
        secsSinec1900 |= (unsigned long)packetbuffer[41] << 16;
        secsSinec1900 |= (unsigned long)packetbuffer[42] << 8;
        secsSinec1900 |= (unsigned long)packetbuffer[43];
        return secsSinec1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    } else {
        Serial.println("No NTP Response");
        return 0;
    }
}

void showTime()
{
   u8g2.setFont(u8g2_font_ncenB08_tr);
   //u8g2.clearBuffer(); 
   int w = u8g2.getMaxCharWidth();
   u8g2.drawStr(0,0, (String(year())+ "/" + String(month()) + "/" + String(day()) ).c_str());
   u8g2.drawStr(w * 5 ,0, (String(hour())+ ":" + String(minute()) + ":" + String(second()) ).c_str());
}

void getWeather()
{
    
    Serial.println("\n Starting connection to server...");

    if(client.connect(openWeatherServer,80))
    {
        Serial.println("connect to server.");

        //http poroccol
        client.print("GET /data/2.5/weather?");
        client.print("&q=" + location);
        client.print("&appid=" + apikey);
        client.println("&units=metric");
        client.println("HOST: api.openweathermap.org");
        client.println("Connection: close");
        client.println();
        client.flush();
    }

    delay(500);
    while(client.available()){
        String JsonStr = client.readStringUntil('\r');
        Serial.println(JsonStr);

        DeserializationError error = deserializeJson(doc,JsonStr);

        if(error){
            Serial.println(F("deserializeJson() failed!"));
            Serial.println(error.c_str());
        }

      

      String ss1 = doc["main"]["temp"];
      String ss2 = doc["main"]["temp_min"];
      String ss3 = doc["main"]["temp_max"];
      String ss4 = doc["main"]["humidity"];
      String ss5 = doc["weather"][0]["main"];
      if (ss1 != NULL){
          TempValue = ss1;
          minTemp = ss2;
          maxTemp = ss3;
          humidityStr = ss4;
          weatherStr = ss5;

      }

      doc.clear();
      if(weatherStr == "Clear"){
          weatherType = 0;
      }

      if(weatherStr == "Clouds"){
          weatherType = 2;
      }

      if(weatherStr == "Rain"){
          weatherType = 3;
      }

      if(weatherStr == "Drizzle"){
          weatherType = 3;
      }

      if(weatherStr == "Thunderstorm"){
          weatherType = 4;
      }           
    }
    client.stop();
             
}

void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t Type)
{
    Serial.print("Symbol:");
    Serial.println(Type);
    switch(Type){
        case SUN:
            u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
            u8g2.drawGlyph(x, y, 69);            
        break;

        case SUN_CLOUD:
            u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
            u8g2.drawGlyph(x, y, 65);         
        break;

        case CLOUD:
            u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
            u8g2.drawGlyph(x, y, 64);         
        break;  

        case RAIN:
            u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
            u8g2.drawGlyph(x, y, 67);         
        break;   

        case THUNDER:
            u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
            u8g2.drawGlyph(x, y, 67);         
        break;                   
    }
}


void reconnectWiFi()
{
    
    WiFi.begin(ssid,pass);
    ConnectTimeOut = millis();

    while(WiFi.status() != WL_CONNECTED){
        Serial.print("Attempting to connect to SSID:");
        Serial.println(ssid);
        
        u8g2.clearBuffer();
        u8g2.setCursor(0,fontHigh);
        u8g2.print("網路連線中...");
        u8g2.sendBuffer();

        if((millis() - ConnectTimeOut) > 10000)
            break;        
        delay(3000);            
    }

    status = WiFi.status();
    if (status == WL_CONNECTED)
     {
        u8g2.clearBuffer();
        u8g2.setCursor(0,fontHigh);
        u8g2.print("連線成功");
        u8g2.sendBuffer();   

        udp.begin(localPort);
        Serial.println("waiting for sync");
        setSyncProvider(getNtpTime);           
        delay(10);
        getWeather();       
    } 
    else
     {
        u8g2.clearBuffer();
        u8g2.setCursor(0,fontHigh);
        u8g2.print("連線失敗");
        u8g2.sendBuffer();
    }
    
}

void setup()
{
	  Serial.begin(115200);

    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese);
    u8g2.setFontPosTop();
    fontWitdh = u8g2.getMaxCharWidth();
    fontHigh = u8g2.getMaxCharHeight();

    u8g2.clearBuffer();
    u8g2.setCursor(0, fontHigh);
    u8g2.print("初始化中...");
    u8g2.sendBuffer();

   //初始化 dht20
   while(dht20.begin()){
      Serial.println("Initialize sensor failed");
      delay(1000);
   }

    reconnectWiFi();
       
}

void loop()
{

   CurrentTime = millis();
   if(CurrentTime - preTime > intervalSwitch){
	preTime = CurrentTime;
    
    
    u8g2.setFont(u8g2_font_unifont_t_chinese);

    u8g2.clearBuffer();
    u8g2.setCursor(0,fontHigh);
    u8g2.print("溫度:");
    u8g2.setCursor(0,fontHigh * 2);
    u8g2.print("濕度:");   
    u8g2.setCursor(0,fontHigh * 3);
    u8g2.print("預報:");     

    TempVale = dht20.getTemperature();  //讀取溫度
    Serial.print("Temp: ");    //將結果印出來
    Serial.print(TempVale);
    Serial.println("C");
    u8g2.setCursor((fontWitdh * 2) + 8,fontHigh);
    u8g2.print(TempVale);

    HumidityValue = dht20.getHumidity() * 100; //讀取濕度
    Serial.print("Humidity: "); //將結果印出來
    Serial.print(HumidityValue);
    Serial.println("%");
    u8g2.setCursor(fontWitdh * 2 + 8,fontHigh * 2);
    u8g2.print(HumidityValue);

   

    if(TempValue != nullptr)
    {
      u8g2.setCursor(fontWitdh * 2 + 8,fontHigh * 3);
      u8g2.print(TempValue); 
      drawWeatherSymbol(88,15,weatherType);
    }
        
    
    showTime();
    
    u8g2.sendBuffer();    
   }	


    if (status == WL_CONNECTED){
        if(minute() % 5 == 0 && second() == 0){   
            getWeather();
        }

                           
    } 

   reTryConnTime = millis();
   if(reTryConnTime - reTryConnPreTime > reTryConnIntervalSwitch){
	reTryConnPreTime = reTryConnTime;

    if(WiFi.status() !=WL_CONNECTED )
    {
        reconnectWiFi();
    }

   }

}
