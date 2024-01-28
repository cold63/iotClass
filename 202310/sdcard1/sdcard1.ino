#include "SPI.h" 
#include "SdFat.h"

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

void setup()
{
  Serial.begin(115200);
  // Wait for USB Serial
  while (!Serial) {
    yield();
  }
  Serial.println("Type any character to start");
  while (!Serial.available()) {
    yield();
  }

  if (!sd.begin(SD_CONFIG)) {
    sd.initErrorHalt();
  }

  uint32_t size = sd.card()->sectorCount();
  uint32_t sizeMB = 0.000512 * size + 0.5;
  Serial.print("sd.vol()->fatType():");
  Serial.println(sd.vol()->fatType());
  Serial.print("Card size: ");
  Serial.print(sizeMB);
  Serial.println(" MB (MB = 1,000,000 bytes)");
  Serial.print("Cluster size (bytes): ");
  Serial.println(sd.vol()->bytesPerCluster());
  sd.ls(LS_R | LS_DATE | LS_SIZE);

  if (!file.open("SoftSPI.txt",O_APPEND| O_RDWR | O_CREAT)) {
    sd.errorHalt(F("open failed"));
   // sd.initErrorHalt(&Serial);
  }
  file.println(F("This line was printed using software SPI."));

  file.rewind();
  
  
  while (file.available()) {
    Serial.write(file.read());
  }

  file.close();

  Serial.println(F("Done."));	
}

void loop()
{
	
}
