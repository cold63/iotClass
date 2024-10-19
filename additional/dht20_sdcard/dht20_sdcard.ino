#include "SPI.h" 
#include "SdFat.h"
#include <DFRobot_DHT20.h>

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

DFRobot_DHT20 dht20;
unsigned long CurrentTime,preTime;
const int intervalSwitch = 1000 * 5;

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

void saveWeatherData()
{

    if(file.open("dht20_data.csv",O_APPEND | O_RDWR | O_CREAT)){

        Serial.println("save weather data to SD CARD.");

        String sd = String(TempVale) + "," + String(HumidityValue);
        
        Serial.println(sd);
        file.println(sd);

        file.rewind();

        delay(1);
        file.close();
    }
}

void setup()
{
	Serial.begin(115200);
    pinMode(saveButton,INPUT_IRQ_FALL);
    digitalSetIrqHandler(saveButton,button_handler);

   while(dht20.begin()){
      Serial.println("Initialize sensor failed");
      delay(1000);
   }

    if (!sd.begin(SD_CONFIG)) {
        sd.initErrorHalt();
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

        HumidityValue = dht20.getHumidity() * 100; //讀取濕度
        Serial.print("Humidity: "); //將結果印出來
        Serial.print(HumidityValue);
        Serial.println("%");      

        if(saveDatFlag){
            saveWeatherData();
        }  
    }
}
