/* 
   Automatic Weather Stasion V.2.0 (PRO)
   Developed by Fadel Arya
   Produced by Smart Agri

   For Contact: instagram.com/smartagri.ugm

   credit:
   - Andri Prima Nugroho
   - Iqbal Maulana Haryadi
   - Ardan Wiratmoko
   - Sandi Firdaus
*/

#include <Arduino.h>
#include "aws/aws.h"
#include "run/run.h"
#include "modbus/ModbusMaster232.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include <esp_task_wdt.h>
#include <String.h>
#include <uRTCLib.h>
#include <SoftwareSerial.h>
#include <Adafruit_INA219.h>
#include "Declare.h"
#include "mode_handler/ModeHandler.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>
#include <ESP32httpUpdate.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <time.h>

#define FIRMWARE_VERSION 2.0
#define TYPE "pro"
#define UPDATE_URL "https://fadelaryap.github.io/ujicoba.github.io/firmware.json"

File myfile2;

uRTCLib rtc(0x68);
sd_card sd_card_(2,3);
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, Serial);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

//----Variabel Deep Sleep----//
int pengkali = 1000000;
int durasisleep = 300;

int nilaii1, nilaii2, nilaii3, nilaii4, nilaii5;

//-----Deklarasi SSID & AP Access Point-----//
const char *SSID_AP = "AWS-Node-01";
const char *PWD_AP = "qwerasdf";

//----Deklarasi RX TX Soft Serial---//
SoftwareSerial SIM800L(32,33);


//--Deklarasi TinyGSM--//
TinyGsmClient client2(modem);

Adafruit_INA219 ina219;

//--Deklarasi sensor AWS--//
Aws aws1(102, Serial2); //Temperature, Humidity, Pressure
Aws aws2(200, Serial2); //Wind Speed & Wind Direction
Aws aws3(202, Serial2); //Rainfall
Aws aws4(204, Serial2); //Radiation
Aws aws5(138, Serial2); //PM 2.5 & PM 10

ModeHandler mode(BUTTON_PIN, LED_PIN);
NodeClient client(server7, key2);

//=--ApiKey, server, dan port---//
String key = "";
const char* server = "http://api.smartagri.id";
const char server2[] = "http://api.smartagri.id";

const char* server3 = "http://agrieye.tp.ugm.ac.id";
const char server4[] = "http://agrieye.tp.ugm.ac.id";

uint8_t port = 80;    

//--Start Webserver--///
AsyncWebServer serverService(80);


/* Attach ESP-DASH to AsyncWebServer */
ESPDash dashboard(&serverService, true);

bool is_first = true;

//--Deklarasi nilai status maintanance--//
bool maintananceState = false;

//---Status Sinyal sebagai penentu kirim ke server---//
bool kirimcloud = false;

int ind = 0;
// int debug_time = 0.5*60*1000;
int delay_time = 2000;
int delay_offline = 30000;
int sendDelay = 5 * 60 * 1000;
// int sendDelay = 10 * 1000;
int previous_time = 0;
int previous_time2 = 0;
int statusGPRS;

void setupTime();
void send_asQuicksend();
void connectWifi();
void cekUpdate();
t_httpUpdate_return UpdateFirmware(String url_update);

//--Menyimpan nilai bacaan sensor apabila tidak terdapat sinyal--//
void logSDCard() {
  dataMessage = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(result[2]) + "&C1=" + 
                String(result[3]) + "&C2=" + String(result[4]) + "&C3=" + String(result[5]) + "&C4=" + String(result[6]) + "&C5=" + String(result[7]) + "&C6=" + String(result[8]) + "&C10=" + String(result[9]) + "&C11=" + String(result[10]) + "&C12=" + String(voltage) + "&C13=" + String(power) + "&R=1" + '\n';
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  sd_card_.appendFile(SD, "/datasensor.txt", dataMessage.c_str());
}

//--Menyimpan nilai sensor untuk websocket local web server--//
void logSDCard2() {
  char bif[20];
  for(int l=1;l<10;l++){
    sprintf(bif, "/SENS%d.txt",l);
    dataMessage = String(result[l+1]);
    Serial.print("Save data: ");
    Serial.println(dataMessage);
    sd_card_.writeFile(SD,bif,dataMessage.c_str());
    delay(500);
    // appendFile(SD, bif, dataMessage.c_str());
  }

  String data_voltage = String(voltage);
  String data_power = String(power);

  sd_card_.writeFile(SD,"/VOLTAGE.txt", data_voltage.c_str());
  sd_card_.writeFile(SD,"/DAYA.txt", data_power.c_str());
}

//---Menyimpan nilai RSSI untuk web socket---//
void logSDCard3() {
  String buf_quality = String(rssiPercentage);
  Serial.println(buf_quality);
  sd_card_.writeFile(SD, "/RSSI.txt",buf_quality.c_str());
  delay(1000);
    // appendFile(SD, bif, dataMessage.c_str());
}

//--Ambil nilai rata-rata dari 9 sampling sensor--//
void result2(){
  float nilai = 0.00;
  for(int cd=2;cd<6;cd++){
    for(int dd = 0;dd<9;dd++){
      nilai += result[cd];
    }
    result[cd] = nilai / 9.00;
    nilai = 0.00;
  }
  for(int cd=7;cd<9;cd++){
    for(int dd = 0;dd<9;dd++){
      nilai += result[cd];
    }
    result[cd] = nilai / 9.00;
    nilai = 0.00;
  }
}

String getFieldFromJson(String payload, const char* fieldName) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);
    JsonObject obj = doc.as<JsonObject>();
    
    if (obj.containsKey(fieldName)) {
        return obj[fieldName].as<String>();
    } else {
        return "null";  // Field not found, return an empty string or handle the error
    }
}

void sendToServer2() {
  Serial.println("Performing HTTP POST request...");
  Serial.println(buffer);

  String req = String(server3) + "/?K=" + key + buffer;
  Serial.println(req);
  Serial.println("Performing HTTP POST request...");
  HTTPClient http;
  WiFiClient wifi;
  
  // Prepare the data to send in JSON format
  bool httpInitResult = http.begin(wifi,req);
  if( httpInitResult == false ) {
      Serial.println("http.begin() failed!"); //debug
  }

  int httpResponseCode = http.GET();
    
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
  }
  else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  sd_card_.appendFile(SD, "/log.txt", payload.c_str());

  if(payload.indexOf("##1") == -1) {
    nilaii1 = 1;
  } else {
    nilaii1 = 2;
  }

  if (nilaii1 == 1) {
    buffer2 = buffer2 + buffer + '\n';
    Serial.println("Data tidak terkirim");
    gagal++;
  }
  else if (nilaii1 == 2) {
    Serial.println("Data dari file terkirim nih");
  }
}

void sendToServer() {
  String req = String(server) + "/?key=" + keyyy + "&C0=" + String(voltage) + "&C1=" + 
               String(result[8]) + "&C2=" + String(result[4]) + "&C3=" + String(power) 
               + "&C4=0.00&C5=" + String(result[2]) + "&C6=" + String(result[3]) + "&C7=" 
               + String(result[7]) + "&C8=" + String(result[5]) + "&C9=" + String(result[6]) 
               + "&C10=" + String(result[9]) + "&C11=" + String(result[10]);
  Serial.println(req);
  Serial.println("Performing HTTP POST request...");
  HTTPClient http;
  WiFiClient wifi;
  
  // Prepare the data to send in JSON format
  bool httpInitResult = http.begin(wifi,req);
  if( httpInitResult == false ) {
      Serial.println("http.begin() failed!"); //debug
  }

  int httpResponseCode = http.GET();
    
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
  }
  else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  sd_card_.appendFile(SD, "/log.txt", payload.c_str());

  String version = getFieldFromJson(payload, "valid");
  Serial.println(version);
  
  if(version == "0" || version == "null"){
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server\n";
    sd_card_.appendFile(SD, "/log.txt", gagalmeng.c_str());
    statekirim = 1;
  } else if(version == "1") {
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    sd_card_.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  }

}



void sendToServer4() {
  String req = String(server) + "/?key=" + keyyy + "&C0=" + String(voltage) + "&C1=" + 
               String(result[8]) + "&C2=" + String(result[4]) + "&C3=" + String(power) 
               + "&C4=0.00&C5=" + String(result[2]) + "&C6=" + String(result[3]) + "&C7=" 
               + String(result[7]) + "&C8=" + String(result[5]) + "&C9=" + String(result[6]) 
               + "&C10=" + String(result[9]) + "&C11=" + String(result[10]);
  Serial.println(req);
  Serial.println("Performing HTTP POST request...");
  HTTPClient http;
  WiFiClient wifi;
  
  // Prepare the data to send in JSON format
  bool httpInitResult = http.begin(wifi,req);
  if( httpInitResult == false ) {
      Serial.println("http.begin() failed!"); //debug
  }

  int httpResponseCode = http.GET();
    
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
  }
  else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  sd_card_.appendFile(SD, "/log.txt", payload.c_str());

  String version = getFieldFromJson(payload, "valid");
  Serial.println(version);
  
  if(version == "0" || version == "null"){
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server lagi\n";
    sd_card_.appendFile(SD, "/log.txt", gagalmeng.c_str());
    statekirim = 2;
  } else if(version == "1") {
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    sd_card_.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  }
}

void sendToServer5() {
  String req = String(server) + "/?key=" + keyyy + "&C0=" + String(voltage) + "&C1=" + 
               String(result[8]) + "&C2=" + String(result[4]) + "&C3=" + String(power) 
               + "&C4=0.00&C5=" + String(result[2]) + "&C6=" + String(result[3]) + "&C7=" 
               + String(result[7]) + "&C8=" + String(result[5]) + "&C9=" + String(result[6]) 
               + "&C10=" + String(result[9]) + "&C11=" + String(result[10]);
  Serial.println(req);
  Serial.println("Performing HTTP POST request...");
  HTTPClient http;
  WiFiClient wifi;
  
  // Prepare the data to send in JSON format
  bool httpInitResult = http.begin(wifi,req);
  if( httpInitResult == false ) {
      Serial.println("http.begin() failed!"); //debug
  }

  int httpResponseCode = http.GET();
    
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
  }
  else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  sd_card_.appendFile(SD, "/log.txt", payload.c_str());

  String version = getFieldFromJson(payload, "valid");
  Serial.println(version);
  
  if(version == "0" || version == "null"){
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server lagi lagi\n";
    sd_card_.appendFile(SD, "/log.txt", gagalmeng.c_str());
    logSDCard();
  } else if(version == "1") {
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    sd_card_.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  }
  
}



String print() {
  String nilaisave = "Sensor 1 = " + String(result[2]) + ", Sensor 2 = " + String(result[3]) + ", Sensor 3 = " + String(result[4]) + ", Sensor 4 = " + String(result[5]) + ", Sensor 5 = " + String(result[6]) + ", Sensor 6 = " + String(result[7]) + ", Sensor 7 = " + String(result[8]) + ", Voltage = " + String(voltage) + "\n";
  return nilaisave;
}

//----------------Membaca config----------------//
void readConfig(){
  a11 = client.setA_Sens1(); a1 = a11.toFloat();delay(100);
  a22 = client.setA_Sens2(); a2 = a22.toFloat();delay(100);
  a33 = client.setA_Sens3(); a3 = a33.toFloat();delay(100);
  a44 = client.setA_Sens4(); a4 = a44.toFloat();delay(100);
  a55 = client.setA_Sens5(); a5 = a55.toFloat();delay(100);
  a66 = client.setA_Sens6(); a6 = a66.toFloat();delay(100);
  a77 = client.setA_Sens7(); a7 = a77.toFloat();delay(100);
  a88 = client.setA_Sens8(); a8 = a88.toFloat();delay(100);
  a99 = client.setA_Sens9(); a9 = a99.toFloat();delay(100);

  c11 = client.setC_Sens1(); c1 = c11.toFloat();delay(100);
  c22 = client.setC_Sens2(); c2 = c22.toFloat();delay(100);
  c33 = client.setC_Sens3(); c3 = c33.toFloat();delay(100);
  c44 = client.setC_Sens4(); c4 = c44.toFloat();delay(100);
  c55 = client.setC_Sens5(); c5 = c55.toFloat();delay(100);
  c66 = client.setC_Sens6(); c6 = c66.toFloat();delay(100);
  c77 = client.setC_Sens7(); c7 = c77.toFloat();delay(100);
  c88 = client.setC_Sens8(); c8 = c88.toFloat();delay(100);
  c99 = client.setC_Sens9(); c9 = c99.toFloat();delay(100);
  String durasi = client.durasi_sleep(); durasisleep = durasi.toInt(); delay(100);
  passwordAP = client.pass_word(); PWD_AP = passwordAP.c_str(); delay(100);
  key = client.setApiFromFile(); delay(100);
}

void setup(){
  Serial.begin(9600);
  SIM800L.begin(9600);
  Serial2.begin(9600);
  
  Serial.println("SD Card Test");
  sd_card_.initSDCard();
  
  // myfile.close();    
  Serial.println("SIM800L GPRS Test");
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  
  URTCLIB_WIRE.begin();
  
  Serial.println("Maintanance button will be read in 5 seconds");
  delay(5000);
  mode.setupButton();
  maintananceState = mode.getMaintananceState(true);
  readConfig();
  aws1.setupRelay(RELAY_PIN);
  if (maintananceState){
    mode.chooseWifiSetting(SSID_AP,PWD_AP);
    serverService.begin();
    aws1.restartSensor(); 
  } 
  else {
    aws1.begin();
    aws2.begin();
    aws3.begin();
    aws4.begin();
    aws5.begin();
    ina219.begin();
    aws1.hidupSensor();
    Serial.println("mencoba connect");
    mode.chooseWifiSetting(SSID,PWD);

    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client; 
      if (!client.connect("www.google.com", 80)) {
        Serial.println("No Internet access available");
      } else {
        Serial.println("internet access available");
        Serial.println("Membaca waktu.....");

        setupTime();
        if(year != -30){
          rtc.set(second,minute,hour,1,day,month,year);
        }
      }
    // delay(5000); // Check every 5 seconds
    }
    
  }

  Serial.println("Connected ");
  Serial.println("Modbus RTU Master Online");
  
  
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);
}

void loop()
{
  esp_task_wdt_reset();
  
  ind++;
  
  if(!maintananceState){
    rtc.refresh();
    Serial.print(String(rtc.hour())); Serial.print(":"); Serial.print(String(rtc.minute())); Serial.print(":"); Serial.println(String(rtc.second()));
    // cekfilehujan();
    // Serial.println(buffer_rain);
    // Serial.println(String(rtc.year()));
    uint8_t bagi = rtc.minute() % 10;
    uint8_t detik2 = rtc.second();
    if(bagi > 6) {

    }
    if((bagi != 0) && (detik2 != 0)){
      delay(1000);
      return;
    }
    else if ((bagi == 0) && (detik2 == 0)){
      tanggal = String(rtc.year())+ '-' +String(rtc.month())+'-'+String(rtc.day());
      tanggal2 = "20" + String(rtc.year())+ '-' +String(rtc.month())+'-'+String(rtc.day());
      waktu = String(rtc.hour())+':'+String(rtc.minute())+':'+String(rtc.second());
      String timestmp = tanggal2 + ' ' + waktu + "\n";
      sd_card_.appendFile(SD, "/log.txt", timestmp.c_str());
      String msg = "Masuk mode WIFI\n";
      sd_card_.appendFile(SD, "/log.txt", msg.c_str());
      result = aws1.get_all();
      result1 = aws2.get_all2();
      result3 = aws3.get_all3();
      result4 = aws4.get_all4();
      result5 = aws5.get_all5();

      result[5] = result1[5];
      result[6] = result1[6];
      result[7] = result3[7];
      result[8] = result4[8];
      result[9] = result5[9];
      result[10] = result5[10];
      

      delay(3000);
      aws1.restartSensor(); 
      voltage = ina219.getBusVoltage_V();
      Serial.println(voltage);
      power = ina219.getPower_mW();
      String nilaisave2 = print();
      sd_card_.appendFile(SD, "/log.txt", nilaisave2.c_str());
            
      logSDCard2();
      sd_card_.cekfiledataraw();
      String jadicsv = String(tanggal2) + ',' + String(waktu) + ',' + String(result[2]) +',' + String(result[3]) +',' + String(result[4]) + ',' + String(result[5]) + ',' + String(result[6]) + ',' + String(result[7]) + ',' + String(result[8]) + ',' + String(result[9]) + ',' + String(result[10]) + ',' + String(voltage) + ',' + String(power) +"\n";
      sd_card_.appendFile(SD, "/dataraw.csv", jadicsv.c_str());
      
      //--Mengkalibrasi nilai sensor--//
      // result[2] = (result[2] * a1) + c1;
      // result[3] = (result[3] * a2) + c2;
      // result[4] = (result[4] * a3) + c3;
      // result[5] = (result[5] * a4) + c4;
      // result[6] = (result[6] * a5) + c5;
      // result[7] = (result[7] * a6) + c6;
      // result[8] = (result[8] * a7) + c7;

      Serial.println("Mencoba Connect");
      Serial.println(key);

      if (WiFi.status() != WL_CONNECTED)
     {
        String tidakConnect = "Tidak tersambung dengan WiFi\n";
        sd_card_.appendFile(SD, "/log.txt", tidakConnect.c_str());
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

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" OK");
        String berhasilGPRS = "Berhasil tersambung dengan WIFI\n";
        sd_card_.appendFile(SD, "/log.txt", berhasilGPRS.c_str());
        WiFiClient client; 
        if (!client.connect("www.google.com", 80)) {
          Serial.println("No Internet access available");
        } else {
          Serial.println("internet access available");
        }
      // delay(5000); // Check every 5 seconds
      } else if (WiFi.status() != WL_CONNECTED) {
        Serial.println("tidak berhasil connect kembali");
        String tidakberhasilConnect = "tidak berhasil kembali connect ke Wifi\n";
        sd_card_.appendFile(SD, "/log.txt", tidakberhasilConnect.c_str());
      }
      
      
      // Serial.print("Connecting to ");
      // Serial.println(server);

      // if (!client2.connect(server2, port)) {
      //   Serial.println(" fail");
      //   logSDCard();
      //   String gagalServer = "Gagal Connect ke Server\n";
      //   sd_card_.appendFile(SD, "/log.txt", gagalServer.c_str());
      // }

      // else {

        // Serial.println(" OK");
        // String berhasilServer = "Berhasil Connect ke Server\n";
        // sd_card_.appendFile(SD, "/log.txt", berhasilServer.c_str());


        Serial.println("Mengirim ke cloud...");
        delay(500);
        sendToServer();
        
        if (statekirim == 1) {
          Serial.println("11");
          sendToServer4();
        }

        if (statekirim == 2) {
          Serial.println("22");
          // logSDCard();
          sendToServer5();
        }

        previous_time2 = millis();
        int counter2 = 1;
        int counter = 1;
        myfile2 = SD.open("/datasensor.txt");
        while(myfile2.available()){
          
          if ((millis() - previous_time2) < delay_offline) {
            int waktudelay = delay_offline - (millis() - previous_time2);
            delay(waktudelay);
          }

          cekfileada = 1;
          
          buffer = myfile2.readStringUntil('\n');
          Serial.println(buffer);
          
          if ((counter2 == 1) || (counter2 == 2)){
            Serial.print("Mengirim ");
            Serial.println(buffer);
            sendToServer2();
            previous_time2 = millis();
            buffer="";
            
          } else {
            buffer3 = buffer3 + buffer + '\n';
          }
          
          counter2++;
          counter++;
            
        }

        Serial.println("1");
        
        myfile2.close();

        SD.remove("/datasensor.txt");
        
        if((buffer3=="") && (buffer2!="")) {
          sd_card_.writeFile(SD, "/datasensor.txt", buffer2.c_str());
        } 
        else if ((buffer2=="") && (buffer3!="")) {
          sd_card_.writeFile(SD, "/datasensor.txt", buffer3.c_str());
        }
        else if ((buffer3 != "") && (buffer2 != "")){
          sd_card_.writeFile(SD, "/datasensor.txt", buffer3.c_str());
          sd_card_.appendFile(SD, "/datasensor.txt", buffer2.c_str());
        }
        
        if (gagal != 0) {
            String gagalConnect2 = "Gagal mengirim lagi sebanyak " + String(gagal) + " lagi\n";
            sd_card_.appendFile(SD, "/log.txt", gagalConnect2.c_str());
        }

        if (cekfileada == 0){
          String adafile = "Masih ada file blm terkirim\n";
          sd_card_.appendFile(SD, "/log.txt", adafile.c_str());
        }

        
        String hapus = "File telah dihapus\n";
        sd_card_.appendFile(SD, "/log.txt", hapus.c_str());
          
        // }
      
      Serial.println("");
      aws1.print_all(result);

      Serial.println("Checking Update...");
      cekUpdate();
      
      esp_sleep_enable_timer_wakeup(pengkali * durasisleep);
      String mulaisleep = "Mulai sleep...\n\n";
      sd_card_.appendFile(SD, "/log.txt", mulaisleep.c_str());
      Serial.println("Going to sleep now");
      delay(1000);
      Serial.flush(); 
      esp_deep_sleep_start();

    }
  }
  else {
    delay(5000);
    quicksend = sd_card_.cekfilequicksend();
    if(quicksend == "1"){
      // serverService.end();
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      Serial.print("Connecting to ");
      connectWifi();

      aws1.hidupSensor();
      Serial.println("Menyalakan sensor selama 10 detik....");
      delay(10000); //delay 10 detik untuk warm up sensor
      Serial.println("Membaca sensor...");

      result = aws1.get_all();
      result1 = aws2.get_all2();
      result3 = aws3.get_all3();
      result4 = aws4.get_all4();
      result5 = aws5.get_all5();

      result[5] = result1[5];
      result[6] = result1[6];
      result[7] = result3[7];
      result[8] = result4[8];
      result[9] = result5[9];
      result[10] = result5[10];
      
      delay(3000);
      aws1.restartSensor(); 

      send_asQuicksend();
    }
    
    // Get and print RSSI
    int rssi = WiFi.RSSI();

    // Convert RSSI to percentage based on your observed range
    rssiPercentage = client.mapRSSItoPercentage(rssi, -100, -30);
    logSDCard3();
    // sd_card_.cekfile3();
    // sd_card_.cekfile2();
    aws1.restartSensor(); //Matiin relay dan sensor
  }
}

t_httpUpdate_return updateFirmware(String url_update)
{
  t_httpUpdate_return ret;

  if(WiFi.status() == WL_CONNECTED)
  {
    ret = ESPhttpUpdate.update(url_update);
    switch (ret)
    {
      case HTTP_UPDATE_FAILED:
        Serial.println("[update] update failed.");
        return ret;
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update");
        return ret;
        break;

      case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok.");
        return ret;
        break;
    }
  }
}

void cekUpdate(){
  HTTPClient http;
  String response;
  String url = UPDATE_URL;
  http.begin(url);
  http.GET();
  response = http.getString();
  Serial.println(response);

  String version = getFieldFromJson(response, "version");
  String url_update = getFieldFromJson(response, "url");

  Serial.println(version);
  Serial.println(url_update);

  if(version.toDouble()>FIRMWARE_VERSION) {
    Serial.println("Update Available");
    if(updateFirmware(url_update) == HTTP_UPDATE_OK)
    {
      Serial.println("Update Success");
      ESP.restart();
    } else {
      Serial.println("Update Failed");
    }

  } else {
    Serial.println("Update not Available");
  }
}



void send_asQuicksend(){
  String req = String(server) + "/?key=" + keyyy + "&C0=" + String(voltage) + "&C1=" + 
               String(result[8]) + "&C2=" + String(result[4]) + "&C3=" + String(power) 
               + "&C4=0.00&C5=" + String(result[2]) + "&C6=" + String(result[3]) + "&C7=" 
               + String(result[7]) + "&C8=" + String(result[5]) + "&C9=" + String(result[6]) 
               + "&C10=" + String(result[9]) + "&C11=" + String(result[10]);
  Serial.println(req);
  Serial.println("Performing HTTP POST request...");
  HTTPClient http;
  WiFiClient wifi;
  
  // Prepare the data to send in JSON format
  bool httpInitResult = http.begin(wifi,req);
  if( httpInitResult == false ) {
      Serial.println("http.begin() failed!"); //debug
  }

  int httpResponseCode = http.GET();
    
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
  }
  else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  sd_card_.appendFile(SD, "/log.txt", payload.c_str());

  String version = getFieldFromJson(payload, "valid");
  Serial.println(version);

  if(version == "0" || version == "null"){
    Serial.println("Gagal mengirim");
  } else if(version == "1") {
    Serial.println("Berhasil mengirim");
    String tes = "0";
    sd_card_.writeFile(SD,"/quicksend.txt",tes.c_str());
    
    delay(3000);
    ESP.restart();
    // serverService.begin();
  }
}

void setupTime(){
  // Configure NTP client
  configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
  delay(10000);
  time_t now;
  struct tm timeinfo;

  // Update time
  time(&now);
  localtime_r(&now, &timeinfo);

  year = timeinfo.tm_year + 1900;
  year-=2000;
  month = timeinfo.tm_mon + 1;
  day = timeinfo.tm_mday;
  hour = timeinfo.tm_hour;
  minute = timeinfo.tm_min;
  second = timeinfo.tm_sec;
  if(second>59){
    second = 0;
  }
  // second-=2;

  Serial.print("Current time (WIB): "); Serial.print(year);Serial.print("-");Serial.print(month);
  Serial.print("-");Serial.print(day);Serial.print(" ");Serial.print(hour);Serial.print(":");
  Serial.print(minute);Serial.print(":");Serial.println(second);

  if(year == -30) {
    Serial.println("Waktu tidak tepat");
    configTime(gmtOffsetSec, daylightOffsetSec, ntpServer);
    delay(7000);
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    year = timeinfo.tm_year + 1900;
    year-=2000;
    month = timeinfo.tm_mon + 1;
    day = timeinfo.tm_mday;
    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    second = timeinfo.tm_sec;
  // second-=2;

    Serial.print("Current time (WIB): ");
    Serial.print(year);
    Serial.print("-");
    Serial.print(month);
    Serial.print("-");
    Serial.print(day);
    Serial.print(" ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(second);
  }
}

void connectWifi(){
  
  Serial.println(SSID);
  WiFi.begin(SSID, PWD);
  Serial.println("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
    {
      // Check to see if connecting failed.
      // This is due to incorrect credentials
      if (WiFi.status() == WL_CONNECT_FAILED)
      {
          Serial.println("Failed to connect to WIFI. Please verify credentials: ");
          Serial.println();
          Serial.print("SSID: ");
          Serial.println(SSID);
          Serial.print("Password: ");
          Serial.println(PWD);
          Serial.println();
      }
      delay(1000);
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Hello World, I'm connected to the internets!!");
}
