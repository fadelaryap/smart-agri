#include "modehandler.h"

#include <WiFiClient.h>
#include <uRTCLib.h>
#include "datamanagement/datamanagement.h"

uRTCLib rtc2(0x68);
WiFiClient client3;
datamanagement data3(8);

modehandler::modehandler(int _buttonPin, int _ledPin)
{
    this->buttonPin = _buttonPin;
    this->ledPin = _ledPin;
    this->isMaintenance = false;
}

void modehandler::setupButton()
{
    pinMode(this->buttonPin, INPUT);
    pinMode(this->ledPin, OUTPUT);
}

bool modehandler::getMaintenanceState(bool update)
{
    if (update)
    {
        int buttonState = analogRead(this->buttonPin);
        
        if (buttonState == LOW)
        {
            this->isMaintenance = false;
            digitalWrite(this->ledPin, HIGH);
            Serial.println("Maintanance State is True!");
        }
        else
        {
            this->isMaintenance = true;
            digitalWrite(this->ledPin, LOW);
            Serial.println("Maintanance State is False!");
        }
    }

    return this->isMaintenance;
}

void modehandler::chooseWifiSetting(const char *ssid, const char *password)
{
    if (this->isMaintenance)
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssid, password);
        Serial.print("IP Address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        Serial.println("Connecting...");
        int counter = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            // Check to see if connecting failed.
            // This is due to incorrect credentials
            if (WiFi.status() == WL_CONNECT_FAILED)
            {
                Serial.println("Failed to connect to WIFI. Please verify credentials: ");
                Serial.println();
                Serial.print("SSID: ");
                Serial.println(ssid);
                Serial.print("Password: ");
                Serial.println(password);
                Serial.println();
            }
            delay(1000);
            counter++;
            if(counter == 120) {
                break;
            }
        }

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        Serial.println("Hello World, I'm connected to the internets!!");
    }
}

void modehandler::setupTime()
{
  const char* ntpServer = "pool.ntp.org";
  const long gmtOffsetSec = 7 * 3600;  // GMT offset for Western Indonesia Time (WIB)
  const int daylightOffsetSec = 0;

  configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
  delay(10000);
  time_t now;
  struct tm timeinfo;

  // Update time
  time(&now);
  localtime_r(&now, &timeinfo);

  this->year = timeinfo.tm_year + 1900;
  this->year-=2000;
  this->month = timeinfo.tm_mon + 1;
  this->day = timeinfo.tm_mday;
  this->hour = timeinfo.tm_hour;
  this->minute = timeinfo.tm_min;
  this->second = timeinfo.tm_sec;
  if(this->second>59){
    this->second = 0;
  }
  // second-=2;

  Serial.print("Current time (WIB): "); Serial.print(this->year);Serial.print("-");Serial.print(this->month);
  Serial.print("-");Serial.print(this->day);Serial.print(" ");Serial.print(this->hour);Serial.print(":");
  Serial.print(this->minute);Serial.print(":");Serial.println(this->second);

  if(this->year == -30) {
    Serial.println("Waktu tidak tepat");
    configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
    delay(7000);
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    this->year = timeinfo.tm_year + 1900;
    this->year-=2000;
    this->month = timeinfo.tm_mon + 1;
    this->day = timeinfo.tm_mday;
    this->hour = timeinfo.tm_hour;
    this->minute = timeinfo.tm_min;
    this->second = timeinfo.tm_sec;
  // second-=2;

    Serial.print("Current time (WIB): ");
    Serial.print(this->year);
    Serial.print("-");
    Serial.print(this->month);
    Serial.print("-");
    Serial.print(this->day);
    Serial.print(" ");
    Serial.print(this->hour);
    Serial.print(":");
    Serial.print(this->minute);
    Serial.print(":");
    Serial.println(this->second);
  }
}

void modehandler::cekwaktu()
{
    if (WiFi.status() == WL_CONNECTED)
    {
      if(!client3.connect("www.google.com",80))
      {
        Serial.println("No Internet access available");
      } else
      {
        Serial.println("internet access available");
        Serial.println("Membaca waktu.....");
        setupTime();
        if(year != -30){
          rtc2.set(second,minute,hour,1,day,month,year);
        }
      }
    }
}

void modehandler::tryConnecting(const char *SSID, const char *PWD)
{
  if (WiFi.status() != WL_CONNECTED)
     {
        String tidakConnect = "Tidak tersambung dengan WiFi\n";
        data3.appendFile(SD, "/log.txt", tidakConnect.c_str());
        Serial.println("Wifi disconnected, reconnecting...");
        WiFi.disconnect();
        delay(500);
        Serial.println("Mencoba connect ulang...");
        WiFi.begin(SSID,PWD);
        int counter = 0;
        while (WiFi.status() != WL_CONNECTED){
          delay(1000);
          Serial.println("Connecting to WiFi...");
          counter++;
          if(counter == 10) {
            break;
          }
        }
      
      }

}

int modehandler::mapRSSItoPercentage(int rssi, int rssiMin, int rssiMax){
  int percentage = map(rssi, rssiMin, rssiMax, 0, 100);
  percentage = constrain(percentage, 0, 100);
  return percentage;
}
