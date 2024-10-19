#include "SPI.h" 
#include "SdFat.h"
#include <U8g2lib.h>
#include <DFRobot_DHT20.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 0
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CS_PIN = 9;
//
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 11;
const uint8_t SOFT_MOSI_PIN = 12;
const uint8_t SOFT_SCK_PIN = 10;

// SdFat software SPI template
SoftSpiDriver<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> softSpi;
// Speed argument is ignored for software SPI.
#if ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(1), &softSpi)
#else  // ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(0), &softSpi)
#endif  // ENABLE_DEDICATED_SPI

#if SD_FAT_TYPE == 0
SdFat sd;
File file;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile file;
#else  // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif  // SD_FAT_TYPE

char c_fileName[100];

int status = WL_IDLE_STATUS;
char ssid[] = "your_ssid";
char pass[] = "your_password";
IPAddress localip;

//********** Time ********************************************
char timeServer[] = "time.stdtime.gov.tw";
const int timeZone = 8;

WiFiUDP udp;
unsigned int localPort = 2390;

const int NTP_PACKET_SIZE = 48;
byte packetbuffer[NTP_PACKET_SIZE];
//time_t prevDisplay = 0;
/////Time End

//************************ open weather map **************************
String apikey ="1b7c10d8003ac6ee7f117f8f8e4fb70a";
String location ="New Taipei City,TW";
char openWeatherServer[] = "api.openweathermap.org";
DynamicJsonDocument doc(3000);

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

const int saveButton = 6;
uint8_t saveDatFlag;

void button_handler(uint32_t id,uint32_t event){
    if(saveDatFlag == 0){
        saveDatFlag = 1;
    }
    else{
        saveDatFlag = 0;
    }
}

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

    delay(300);
    while(client.available()){
        String JsonStr = client.readStringUntil('\r');
        Serial.println(JsonStr);

        DeserializationError error = deserializeJson(doc,JsonStr);

        if(error){
            Serial.println(F("deserializeJson() failed!"));
            Serial.println(error.c_str());
        }
    }

    client.stop();

    String ss1 = doc["main"]["temp"];
    String ss2 = doc["main"]["temp_min"];
    String ss3 = doc["main"]["temp_max"];
    String ss4 = doc["main"]["humidity"];
    String ss5 = doc["weather"][0]["main"];
    TempValue = ss1;
    minTemp = ss2;
    maxTemp = ss3;
    humidityStr = ss4;
    weatherStr = ss5;
    
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

void saveWeatherData()
{
   
    sprintf(c_fileName,"WeatherData_Y%d_D%d.txt",month(),day());
    if(!sd.exists(c_fileName)){
        Serial.println("Create new file.");
        if(file.open(c_fileName, O_RDWR | O_CREAT)){
            file.print(F("時間,感測器-氣溫,感測器-濕度,預報-氣溫,預報-最小溫度,預報-最大溫度,預報-濕度\r\n"));

            file.rewind();
            file.close();
            delay(1);
        }
    }

    if(file.open(c_fileName,O_APPEND | O_RDWR | O_CREAT)){

        Serial.println("save weather data to SD CARD.");

        String sd = String(year()) + "/" + String(month()) + "/" + String(day()) 
        + " " + String(hour()) +":" + String(minute()) + ":" + String(second()) + "," 
        + String(TempVale) + "," + String(HumidityValue) + "," + TempValue + "," + minTemp + "," 
        + maxTemp + "," + humidityStr;
        
        Serial.println(sd);
        file.println(sd);

        file.rewind();

        delay(1);
        file.close();
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
    localip = WiFi.localIP();
}

void setup()
{
	Serial.begin(115200);
    pinMode(saveButton,INPUT_IRQ_FALL);
    digitalSetIrqHandler(saveButton,button_handler);

    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_unifont_t_chinese);
    u8g2.setFontPosTop();
    fontWitdh = u8g2.getMaxCharWidth();
    fontHigh = u8g2.getMaxCharHeight();

    //u8g2.clear();
    u8g2.clearBuffer();
    u8g2.setCursor(0, fontHigh);
    u8g2.print("初始化中...");
    u8g2.sendBuffer();

   //初始化 dht20
   while(dht20.begin()){
      Serial.println("Initialize sensor failed");
      delay(1000);
   }

    if (!sd.begin(SD_CONFIG)) {
       // u8g2.clear();
        u8g2.clearBuffer();
        u8g2.drawUTF8(0,fontHigh,"未發現 SD CARD");
        u8g2.sendBuffer();        
        sd.initErrorHalt();
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
        drawWeatherSymbol(88,15,weatherType);
    
    showTime();

    u8g2.setCursor(0,fontHigh * 3);
    if(saveDatFlag)
    {
        u8g2.setFont(u8g2_font_unifont_t_chinese);
        u8g2.print("資料紀錄中...");
    }
    else
    {
        u8g2.setFont(u8g2_font_ncenB08_tr);
        u8g2.print(localip.get_address());
    }
    
    u8g2.sendBuffer();    
   }	


    if (status == WL_CONNECTED){
        if(minute() % 10 == 0 && second() == 0){   
            getWeather();
        }

        if(saveDatFlag && TempValue != nullptr && minute() % 10 == 0 && second() == 0 ){
            saveWeatherData(); 
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
