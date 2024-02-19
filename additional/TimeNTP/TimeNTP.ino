#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiUdp.h>

int status = WL_IDLE_STATUS;
char ssid[] = "your_ssid";
char pass[] = "your_passwd";

char timeServer[] = "time.stdtime.gov.tw";
const int timeZone = 8;

WiFiUDP udp;
unsigned int localPort = 2390;

const int NTP_PACKET_SIZE = 48;
byte packetbuffer[NTP_PACKET_SIZE];
time_t prevDisplay = 0;

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





void printDigits(int digits){
    Serial.print(":");
    if (digits < 10)
        Serial.print('0');
    Serial.print(digits);    
}

void digitalClockDisplay(){
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(" ");
    Serial.print(day());
    Serial.print(" ");
    Serial.print(month());
    Serial.print(" ");
    Serial.print(year()); 
    Serial.println();     
}

void setup()
{
    Serial.begin(115200);
    Serial.println("TimeNTP Example");

    while(status != WL_CONNECTED){
        Serial.print("Attempting to connect to SSID:");
        Serial.println(ssid);

        status = WiFi.begin(ssid,pass);
        if (status == WL_CONNECTED) break;

        delay(1000);
    }
	
    udp.begin(localPort);
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
}

void loop()
{
    if (timeStatus() != timeNotSet){
        if(now() != prevDisplay){
            prevDisplay = now();
            digitalClockDisplay();
        }
    }
}
