#include <WiFi.h>
#include <U8g2lib.h>
#include <DFRobot_DHT20.h>
#include <HttpClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


#define MQTT_RECONNECT_INTERVAL 100  // millisecond
#define MQTT_LOOP_INTERVAL 800       // millisecond

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

DFRobot_DHT20 dht20;
int YelloLED = 9;

char ssid[] = "your_ssid";     // your network SSID (name)
char pass[] = "your_passwd";  // your network password
int status  = WL_IDLE_STATUS;    // the Wifi radio's status
long lastTime = 0;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
char json_output[300];

unsigned long CurrentTime,preTime;
const int intervalSwitch = 5000;

unsigned long GetDataCurrentTime,GetDatapreTime;
const int GetDataintervalSwitch = 1000;
double TempVale,HumidityValue;


char mqttServer[]     = "iiot.ideaschain.com.tw";
char clientId[]       = "A1PICO888999";
char publishTopic[]   = "v1/devices/me/telemetry";   
char subscribeTopic[] = "v1/devices/me/telemetry";
char subscribeTopicRPC[] = "v1/devices/me/rpc/request/+";  // RPC MQTT

char username[] = "FRgnCF0TDz4LGGTWvuuf";  // device access token(存取權杖)
char password[] = "";                      // no need password
int fontWitdh;

void mqtt_callback(char* topic, byte* payload, unsigned int msgLength) {
  char temp[80];
  JsonDocument mqttTemp;
  sprintf(temp, "Message arrived with Topic [%s]\n  Data Length: [%d], Payload: [", topic, msgLength);
  Serial.print(temp);
  Serial.write(payload, msgLength);
  Serial.println("]");

  deserializeJson(mqttTemp, payload);
  String Str2 = mqttTemp["params"];
  String Str1 = mqttTemp["method"];
  Serial.print("params = ");
  Serial.println(Str2);
  Serial.print("method = ");
  Serial.println(Str1);

  if(Str1 == "setValueOUT")
  {
    if(Str2 == "false")
    {
      digitalWrite(YelloLED, HIGH);
    }
    else
    {
      digitalWrite(YelloLED, LOW);
    }
  }

}

void reconnect()
{

    // Loop until we're reconnected
    while (!(mqttClient.connected())) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqttClient.connect(clientId, username, password)) {
            Serial.println("connected");
            //mqttClient.subscribe(subscribeTopic);
            mqttClient.subscribe(subscribeTopicRPC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
  


}

void sendMQTT()
{
  JsonDocument doc;
  

  doc["Temp"] = TempVale;
  doc["Humi"] = HumidityValue;  
  
  serializeJson(doc, json_output);
  
  if (!mqttClient.connected()) {
      Serial.println("Attempting MQTT connection");
      mqttClient.connect(clientId, username, password);
  }

    if (mqttClient.connected()) {
        mqttClient.publish(publishTopic, json_output);
    }  

}


void setup()
{
	Serial.begin(115200);
  pinMode(YelloLED, OUTPUT);
  digitalWrite(YelloLED, HIGH); 

  u8g2.begin();
  u8g2.setFont(u8g2_font_8x13_mf);
  u8g2.setFontPosTop();
  fontWitdh = u8g2.getMaxCharWidth();

   //初始化 dht20
   while(dht20.begin()){
      Serial.println("Initialize sensor failed");
      delay(1000);
   }

    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }



    mqttClient.setServer(mqttServer, 1883);
    mqttClient.setCallback(mqtt_callback);
    // Allow the hardware to sort itself out
    delay(1500);      
    

   u8g2.clear();
   u8g2.drawStr(0,20,"Temp: ");
   u8g2.drawStr(0,40,"Humidity:");
   u8g2.sendBuffer();    
}

void loop()
{
    if (!(mqttClient.connected())) {
        reconnect();
    }
  
   GetDataCurrentTime = millis();
   if(GetDataCurrentTime - GetDatapreTime > GetDataintervalSwitch){
    GetDatapreTime = GetDataCurrentTime;
    
      TempVale = dht20.getTemperature();  //讀取溫度
      Serial.print("Temp: ");    //將結果印出來
      Serial.print(TempVale);
      Serial.println("C");

      delay(1);

      HumidityValue = dht20.getHumidity() * 100; //讀取濕度
      Serial.print("Humidity: "); //將結果印出來
      Serial.print(HumidityValue);
      Serial.println("%");

      u8g2.setCursor(fontWitdh * 5 + 1,20);
     
      u8g2.updateDisplay();
      delay(1);
      u8g2.setCursor(fontWitdh * 5 + 1,20);
      u8g2.print(TempVale);
      u8g2.print("C");
      u8g2.updateDisplay();
      delay(1);
      u8g2.setCursor(fontWitdh * 9 + 1,40);
     
      u8g2.updateDisplay();   
      delay(1);
      u8g2.setCursor(fontWitdh * 9 + 1,40);
      u8g2.print(HumidityValue);     
      u8g2.updateDisplay();      
      
   }


   CurrentTime = millis();
   if(CurrentTime - preTime > intervalSwitch){
		preTime = CurrentTime;

    sendMQTT();
  
   }

   mqttClient.loop();
}
