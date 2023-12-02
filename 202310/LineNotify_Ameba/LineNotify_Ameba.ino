/*

 Example guide:
 https://www.amebaiot.com/en/amebad-arduino-web-client/
 */

#include <WiFi.h>

char ssid[] = "your-ssid"; // 置換你的 wifi 的 ssid
char pass[] = "your-password";    // 置換你的 wifi 密碼

int status = WL_IDLE_STATUS;
char server[] = "notify-api.line.me";    // 連接網址或是 IP 位址

String LineToken = "your-token";
String message = "I want test line notify!";


WiFiSSLClient client;
void setup() {
    //Initialize serial and wait for port to open:
    Serial.begin(115200);
    while (!Serial) {
        ;
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

    Serial.println("\nStarting connection to server...");

    // 開始連接網路位，使用埠 443
    if (client.connect(server, 443)) {
        Serial.println("connected to server");
        // 使用 HTTP request:
        String query = "message=" + message;
        client.print("POST /api/notify HTTP/1.1\r\n");
        client.print("Host: " + String(server) +"\r\n"); 
        client.print("Authorization: Bearer " + LineToken + "\r\n"); 
        client.print("Content-Type: application/x-www-form-urlencoded\r\n");
        client.print("Content-Length: " + String(query.length()) + "\r\n");
        client.print("\r\n");
       
        client.print(query + "\r\n");       
        //client.println();
        //client.println("Connection: close");
        
    }
    delay(100);
}

void loop() {
    
    // 假如有資料進來就進入 while 內
    while (client.available()) {
        char c = client.read();   //將收到的資料列印出來
        Serial.write(c);
    }

    // 如果服務已斷，將停止接收
    if (!client.connected()) {
        Serial.println();
        Serial.println("disconnecting from server.");
        client.stop();

        // do nothing forevermore:
        while (true);
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
