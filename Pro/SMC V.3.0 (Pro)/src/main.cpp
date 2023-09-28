#include <Arduino.h>
#include <declare.h>
#include <uRTCLib.h>
#include <esp_task_wdt.h>
#include <ESPDash.h>
#include <ESPAsyncWebServer.h>
#include "akuisisidata/akuisisidata.h"
#include "modehandler/modehandler.h"
#include "sendserver/sendserver.h"
#include "datamanagement/datamanagement.h"


akuisisi sensor(SSRPin);
modehandler mode(buttonPin, ledPin);
uRTCLib rtc(0x68);
AsyncWebServer serverService(80);
ESPDash dashboard(&serverService, true);
sendserver send(server,server3,keyyy,key);
datamanagement data(6);

void readConfig();

void setup() {
  Serial.begin(9600);
  sensor.setupSensor();
  mode.setupButton();
  URTCLIB_WIRE.begin();
  data.start();
  readConfig();
  Serial.println("Maintanance button will be read in 5 seconds");
  delay(5000);
  maintenanceState = mode.getMaintenanceState(true);
  if(maintenanceState)
  {
    mode.chooseWifiSetting(SSID_AP, PWD_AP);
    serverService.begin();
    sensor.turnOffSensor();
  } else
  {
    sensor.setupBegin();
    sensor.turnOnSensor();
    Serial.println("Mencoba connect...");
    mode.chooseWifiSetting(SSID,PWD);
    mode.cekwaktu();
    
  }
  Serial.println("Connected ");
  Serial.println("Modbus RTU Master Online");
   
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);
}

void loop() {
  esp_task_wdt_reset();

  if(!maintenanceState){
    rtc.refresh();
    Serial.print(String(rtc.hour())); Serial.print(":"); Serial.print(String(rtc.minute())); Serial.print(":"); Serial.println(String(rtc.second()));
    uint8_t bagi = rtc.minute() % 10;
    uint8_t detik2 = rtc.second();

    if((bagi != 0) && (detik2 != 0)){
      delay(1000);
      return;
    } else if ((bagi == 0) && (detik2 == 0))
    {
      tanggal = String(rtc.year())+ '-' +String(rtc.month())+'-'+String(rtc.day());
      tanggal2 = "20" + String(rtc.year())+ '-' +String(rtc.month())+'-'+String(rtc.day());
      waktu = String(rtc.hour())+':'+String(rtc.minute())+':'+String(rtc.second());

      String timestmp = tanggal2 + ' ' + waktu + "\n";
      data.appendFile(SD, "/log.txt", timestmp.c_str());
      String msg = "Masuk mode WIFI\n";
      data.appendFile(SD, "/log.txt", msg.c_str());

      result = sensor.get_all();
      delay(3000);
      sensor.turnOffSensor();
      
      String nilaisave2 = sensor.print(result);
      data.appendFile(SD, "/log.txt", nilaisave2.c_str());
      data.saveSensor(result);

      data.checkFile("/dataraw.csv");
      String jadicsv = String(tanggal2) + ',' + String(waktu) + ',' + String(result[0]) +',' + String(result[1]) +',' + String(result[2]) + ',' + String(result[3]) + ',' + String(result[4]) + ',' + String(result[5]) + ',' + String(result[6]) + ',' + String(result[7]) + ',' + String(result[8]) + ',' + String(result[9]) + ',' + String(result[10]) +"\n";
      data.appendFile(SD, "/dataraw.csv", jadicsv.c_str());

      Serial.println("Mencoba Connect");
      mode.tryConnecting(SSID,PWD);
      send.trySending(result, tanggal2, waktu);
      sensor.print_all(result);

      // send.cekUpdate(UPDATE_URL, TYPE, FIRMWARE_VERSION);

      esp_sleep_enable_timer_wakeup(pengkali * durasisleep);
      String mulaisleep = "Mulai sleep...\n\n";
      data.appendFile(SD, "/log.txt", mulaisleep.c_str());
      Serial.println("Going to sleep now");
      delay(1000);
      Serial.flush(); 
      esp_deep_sleep_start();
    }
  } else
  {
    delay(5000);
    data.checkFile("/quicksend.txt");
    String quicksend = data.readFile(SD, "/quicksend.txt");
    if(quicksend == "1"){
      send.connect_quicksend(SSID,PWD);
      sensor.setupBegin();
      sensor.turnOnSensor();
      Serial.println("Menyalakan sensor selama 10 detik....");
      delay(10000);
      Serial.println("Membaca sensor...");
      result = sensor.get_all();
      delay(3000);
      sensor.turnOffSensor();
      send.quickSend(result);
    }

    // Get and print RSSI
    int rssi = WiFi.RSSI();
    // Convert RSSI to percentage based on your observed range
    rssiPercentage = mode.mapRSSItoPercentage(rssi, -100, -30);
    data.saveSignalQuality(rssiPercentage);
    sensor.turnOffSensor();
  }
}

void readConfig()
{
  a_temp = data.readCalibration("/a_1.txt");
  c_temp = data.readCalibration("/c_1.txt");

  a_humidity = data.readCalibration("/a_2.txt");
  c_humidity = data.readCalibration("/c_2.txt");

  a_atm_press = data.readCalibration("/a_3.txt");
  c_atm_press = data.readCalibration("/c_3.txt");

  a_wind_speed = data.readCalibration("/a_4.txt");
  c_wind_speed = data.readCalibration("/c_4.txt");

  a_wind_dir = data.readCalibration("/a_5.txt");
  c_wind_dir = data.readCalibration("/c_5.txt");

  a_rainfall = data.readCalibration("/a_6.txt");
  c_rainfall = data.readCalibration("/c_6.txt");

  a_radiance = data.readCalibration("/a_7.txt");
  c_radiance = data.readCalibration("/c_7.txt");

  a_pm25 = data.readCalibration("/a_8.txt");
  c_pm25 = data.readCalibration("/c_8.txt");

  a_pm10 = data.readCalibration("/a_9.txt");
  c_pm10 = data.readCalibration("/c_9.txt");

  durasisleep = data.readSleep("/durasi_sleep.txt");
  keyyy = data.readApiKey("/api_key.txt");
}