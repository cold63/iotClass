/*

 Example guide:
 https://www.amebaiot.com/en/amebad-arduino-web-client/
 */
#include <DFRobot_DHT20.h>
#include <WiFi.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

//DHT20
DFRobot_DHT20 dht20;
unsigned long CurrentTime,preTime;
const int intervalSwitch = 1000;
double TempVale,HumidityValue;
int fontWitdh;

char ssid[] = "your-ssid"; // 置換你的 wifi 的 ssid
char pass[] = "your-password";    // 置換你的 wifi 密碼


int status = WL_IDLE_STATUS;
char server[] = "notify-api.line.me";    // 連接網址或是 IP 位址

String LineToken = "your-token";
String message = "";

WiFiSSLClient client;

//GPIO irq
int button = 6;
bool PushState = 0;

void button_handler(uint32_t id,uint32_t event){
    if(PushState == 0){
      PushState = 1;
    }
}

void setup() {
    //Initialize serial and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {
        ;
    }
    //GPIO irq
    pinMode(button, INPUT_IRQ_FALL);
    digitalSetIrqHandler(button, button_handler);

  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese1);
  u8g2.setFontPosTop();
  fontWitdh = u8g2.getMaxCharWidth();

   u8g2.clear();
   u8g2.clearBuffer();
   u8g2.drawUTF8(0,20,"溫度: ");
   u8g2.drawUTF8(0,40,"濕度: ");
   u8g2.sendBuffer();
   delay(5);

     //初始化 dht20
   while(dht20.begin()){
      Serial.println("Initialize sensor failed");
      delay(1000);
   }
     
    // 確認是否有 wifi 功能
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        // don't continue:
        while (true);
    }

    // 確認是否有連接 WiFi
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // 連接 WiFi
        status = WiFi.begin(ssid, pass);

        // 等待 10 秒
        delay(10000);
    }
    Serial.println("Connected to wifi");
    //列印網路狀態
    printWifiStatus();

}

void loop() {


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

      u8g2.setCursor(fontWitdh * 3 + 1,20);
      u8g2.updateDisplay();
      delay(1);
      u8g2.setCursor(fontWitdh * 3 + 1,20);
      u8g2.print(TempVale);
      u8g2.print("C");
      u8g2.updateDisplay();
      
      u8g2.setCursor(fontWitdh * 3 + 1,40);
      u8g2.updateDisplay();   
      delay(1);
      u8g2.setCursor(fontWitdh * 3 + 1,40);
      u8g2.print(HumidityValue);     
      u8g2.print("%");
      u8g2.updateDisplay();   

   }

    if(PushState == 1){   

    Serial.println("\nStarting connection to server...");

    // 開始連接網路位址，使用埠 443
    if (client.connect(server, 443)) {
        Serial.println("connected to server");
        // 使用 HTTP request:
        message =   "溫度: " + String(TempVale,2) + " 濕度: " + String(HumidityValue,2);
        String query = "message=" + message;
        client.print("POST /api/notify HTTP/1.1\r\n");
        client.print("Host: " + String(server) +"\r\n"); 
        client.print("Authorization: Bearer " + LineToken + "\r\n"); 
        client.print("Content-Type: application/x-www-form-urlencoded\r\n");
        client.print("Content-Length: " + String(query.length()) + "\r\n");
        client.print("\r\n");
       
        client.print(query + "\r\n");       

        
    }

      client.stop();   

      PushState = 0;
    }


}


void printWifiStatus() {
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
