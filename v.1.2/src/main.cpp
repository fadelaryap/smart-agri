/* 
   Automatic Weather Stasion V.1.2
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
Aws aws1(1, Serial2);
ModeHandler mode(BUTTON_PIN, LED_PIN);
NodeClient client(server7, key2);

//=--ApiKey, server, dan port---//
String key = "";
const char* server = "api.smartagri.id";
const char server2[] = "api.smartagri.id";

const char* server3 = "agrieye.tp.ugm.ac.id";
const char server4[] = "agrieye.tp.ugm.ac.id";

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

//--Menyimpan nilai bacaan sensor apabila tidak terdapat sinyal--//
void logSDCard() {
  dataMessage = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(result[2]) + "&C1=" + 
                String(result[3]) + "&C2=" + String(result[4]) + "&C3=" + String(result[5]) + "&C4=" + String(result[6]) + "&C5=" + String(result[7]) + "&C6=" + String(result[8]) + "&R=1" + '\n';
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  sd_card_.appendFile(SD, "/datasensor.txt", dataMessage.c_str());
}

//--Menyimpan nilai sensor untuk websocket local web server--//
void logSDCard2() {
  char bif[20];
  for(int l=1;l<8;l++){
    sprintf(bif, "/SENS%d.txt",l);
    dataMessage = String(result[l+1]);
    Serial.print("Save data: ");
    Serial.println(dataMessage);
    sd_card_.writeFile(SD,bif,dataMessage.c_str());
    delay(500);
    // appendFile(SD, bif, dataMessage.c_str());
  }
}

//---Menyimpan nilai RSSI untuk web socket---//
void logSDCard3() {
  if(quality!=99){
    String buf_quality = String(quality);
    Serial.print("Hasil dari String: ");Serial.println(buf_quality);
    int buffer_quality = buf_quality.toInt();
    Serial.print("Hasil ke Integer: "); Serial.println(buffer_quality);
    float buffer_quality2 = buffer_quality / 32.00 * 100.00;
    Serial.print("Hasil dari dibagi 32: ");Serial.println(buffer_quality2);
    dataMessage3 = String(buffer_quality2);
  } else if (quality == 99){
    dataMessage3 = "Signal Error";
  }
  Serial.println(dataMessage3);
  sd_card_.writeFile(SD, "/RSSI.txt",dataMessage3.c_str());
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

void sendToServer2() {
  Serial.println("Performing HTTP POST request...");
  Serial.println(buffer);
  client2.print("GET /feed/api2.php?K=");
  client2.print(key);
  client2.print(buffer);
  client2.print(" HTTP/1.0\n");
  client2.print("Host: ");
  client2.println(server3);
  client2.print("User-Agent: Arduino-agrieye\n");
  client2.println();
  unsigned long timeout = millis();
  while (client2.connected() && millis() - timeout < 10000L) {
    // Print available data (HTTP response from server)
    while (client2.available()) {
      char cd = client2.read();
      cd2 += cd;
      Serial.println(cd);
      timeout = millis();

      if (cd2.indexOf('#') == -1) {
        nilaii1 = 1;
      }
      else {
        nilaii1 = 2;
      }
      
      // appendFile(SD, "/log.txt", cd.c_str());
    }
  }
  
  if (nilaii1 == 1) {
    buffer2 = buffer2 + buffer + '\n';
    Serial.println("Data tidak terkirim");
  }
  else if (nilaii1 == 2) {
    Serial.println("Data dari file terkirim nih");
  }
  
}

void sendToServer() {
  String requestData = "&C0=" + String(voltage) + "&C1=" + String(result[8]) + "&C2=" + String(result[4]) + "&C3=0.00&C4=0.00&C5=" + String(result[2]) + "&C6=" + String(result[3]) + "&C7=" + String(result[7]) + "&C8=" + String(result[5]) + "&C9=" + String(result[6]);
  Serial.println(requestData); 
  Serial.println("Performing HTTP POST request...");
  client2.print("GET ");
  // String buatkey = "/feed/api2.php?K=" + String(key) + String(requestData);
  // client2.print(String("GET ") + buatkey + " HTTP/1.0\r\n");
  client2.print("/?key=");
  client2.print(keyyy);
  client2.print(requestData);
  client2.print(" HTTP/1.0\n");
  // client2.print(String("Host: ") + server + "\r\n");
  client2.print("Host: ");
  client2.println(server);
  // String useragent = "Arduino-agrieye";
  // client2.print(String("User-Agent: ") + useragent + "\r\n");
  // client2.print("Connection: close\r\n\r\n");
  client2.print("User-Agent: Arduino-agrieye\n");
  client2.println();

  unsigned long timeout = millis();
  while (client2.connected() && millis() - timeout < 20000L) {
    // Print available data (HTTP response from server)
    while (client2.available()) {
      char ccd = client2.read();
      Serial.println(ccd);
      smt1 = String(ccd);
      simpann += ccd;
      if (simpann.indexOf('{')== -1){
        nilaii3 = 1;
      } else {
        nilaii3 = 2;
      }
      ccd2 += ccd;
      timeout = millis();
      
    }
  }
  sd_card_.appendFile(SD, "/log.txt", ccd2.c_str());
  if (nilaii3 == 2){
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    sd_card_.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  } 
  else if(nilaii3 == 1){
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server\n";
    sd_card_.appendFile(SD, "/log.txt", gagalmeng.c_str());
    statekirim = 1;
  }
}



void sendToServer4() {
  String requestData = "&C0=" + String(voltage) + "&C1=" + String(result[8]) + "&C2=" + String(result[4]) + "&C3=0.00&C4=0.00&C5=" + String(result[2]) + "&C6=" + String(result[3]) + "&C7=" + String(result[7]) + "&C8=" + String(result[5]) + "&C9=" + String(result[6]);
  Serial.println(requestData); 
  Serial.println("Performing HTTP POST request...");
  client2.print("GET ");
  // String buatkey = "/feed/api2.php?K=" + String(key) + String(requestData);
  // client2.print(String("GET ") + buatkey + " HTTP/1.0\r\n");
  client2.print("/?key=");
  client2.print(keyyy);
  client2.print(requestData);
  client2.print(" HTTP/1.0\n");
  // client2.print(String("Host: ") + server + "\r\n");
  client2.print("Host: ");
  client2.println(server);
  // String useragent = "Arduino-agrieye";
  // client2.print(String("User-Agent: ") + useragent + "\r\n");
  // client2.print("Connection: close\r\n\r\n");
  client2.print("User-Agent: Arduino-agrieye\n");
  client2.println();

  unsigned long timeout = millis();
  while (client2.connected() && millis() - timeout < 10000L) {
    // Print available data (HTTP response from server)
    while (client2.available()) {
      char ccd = client2.read();
      Serial.println(ccd);
      smt2 = String(ccd);
      ccd2 += ccd;
      timeout = millis();
      
      if (ccd2.indexOf('{') == -1) {
        nilaii4 = 1;
      }
      else {
        nilaii4 = 2;
      }
    }
  }

  sd_card_.appendFile(SD, "/log.txt", ccd2.c_str());
  if (nilaii4 == 1) {
    statekirim = 2;
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server lagi\n";
    sd_card_.appendFile(SD, "/log.txt", gagalmeng.c_str());
  }
  else if(nilaii4 == 2) {
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    sd_card_.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  }

  
  
}

void sendToServer5() {
  String requestData = "&C0=" + String(voltage) + "&C1=" + String(result[8]) + "&C2=" + String(result[4]) + "&C3=0.00&C4=0.00&C5=" + String(result[2]) + "&C6=" + String(result[3]) + "&C7=" + String(result[7]) + "&C8=" + String(result[5]) + "&C9=" + String(result[6]);
  Serial.println(requestData); 
  Serial.println("Performing HTTP POST request...");
  client2.print("GET ");
  // String buatkey = "/feed/api2.php?K=" + String(key) + String(requestData);
  // client2.print(String("GET ") + buatkey + " HTTP/1.0\r\n");
  client2.print("/?key=");
  client2.print(keyyy);
  client2.print(requestData);
  client2.print(" HTTP/1.0\n");
  // client2.print(String("Host: ") + server + "\r\n");
  client2.print("Host: ");
  client2.println(server);
  // String useragent = "Arduino-agrieye";
  // client2.print(String("User-Agent: ") + useragent + "\r\n");
  // client2.print("Connection: close\r\n\r\n");
  client2.print("User-Agent: Arduino-agrieye\n");
  client2.println();

  unsigned long timeout = millis();
  while (client2.connected() && millis() - timeout < 10000L) {
    // Print available data (HTTP response from server)
    while (client2.available()) {
      char ccd = client2.read();
      Serial.println(ccd);
      smt3 = String(ccd);
      ccd2 += ccd;
      timeout = millis();
      
      if (ccd2.indexOf('{') == -1) {
        nilaii5 = 1;
      }
      else {
        nilaii5 = 2;
      }
    }
  }

  sd_card_.appendFile(SD, "/log.txt", ccd2.c_str());

  if (nilaii5 == 1) {
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server lagi lagi\n";
    sd_card_.appendFile(SD, "/log.txt", gagalmeng.c_str());
    logSDCard();
  }
  else if(nilaii5 == 2){
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
    modem.init();
    aws1.begin();
    // ina219.begin();
    aws1.hidupSensor();
    // voltage = ina219.getBusVoltage_V();
    //   Serial.println(voltage);
    // delay(90000);
    Serial.println("mencoba connect");
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      Serial.println(" fail");
    }
    else {
      Serial.println(" OK");
      Serial.println("Membaca waktu");

      int   year3    = 0;
      int   month3   = 0;
      int   day3     = 0;
      int   hour3    = 0;
      int   min3     = 0;
      int   sec3     = 0;
      float timezone = 0;

      if (modem.getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3, &timezone)) {
        Serial.print("Year:"); Serial.print(year3); Serial.print("\tMonth:"); Serial.print(month3);  Serial.print("\tDay:"); Serial.println(day3);
        Serial.print("Hour:");Serial.print(hour3); Serial.print("\tMinute:"); Serial.print(min3); Serial.print("\tSecond:"); Serial.println(sec3);
        Serial.print("Timezone:"); Serial.println(timezone);
        int year4 = year3;
        year3 -= 2000;
        sec3 += 1;
        rtc.set(sec3,min3,hour3,1,day3,month3,year3);
      }
      modem.gprsDisconnect();
      Serial.println(F("GPRS disconnected"));
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
      String msg = "Masuk mode GPRS\n";
      sd_card_.appendFile(SD, "/log.txt", msg.c_str());
      result = aws1.get_all();
      // result2();
      delay(3000);
      aws1.restartSensor(); 
      // voltage = ina219.getBusVoltage_V();
      // Serial.println(voltage);
      // power = ina219.getPower_mW();
      String nilaisave2 = print();
      sd_card_.appendFile(SD, "/log.txt", nilaisave2.c_str());
            
      logSDCard2();
      sd_card_.cekfiledataraw();
      String jadicsv = String(tanggal2) + ',' + String(waktu) + ',' + String(result[2]) +',' + String(result[3]) +',' + String(result[4]) + ',' + String(result[5]) + ',' + String(result[6]) + ',' + String(result[7]) + ',' + String(result[8]) + ',' + String(voltage) +"\n";
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

      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" fail to Connect to GPRS");
        sd_card_.cekfile();
        logSDCard();
        String gagalconnect = "Gagal Connect ke GRPS\n";
        sd_card_.appendFile(SD, "/log.txt", gagalconnect.c_str());
      }

      else {
        Serial.println(" OK");
        String berhasilGPRS = "Berhasil tersambung dengan GPRS\n";
        sd_card_.appendFile(SD, "/log.txt", berhasilGPRS.c_str());
        Serial.print("Connecting to ");
        Serial.println(server);

        if (!client2.connect(server2, port)) {
          Serial.println(" fail");
          logSDCard();
          String gagalServer = "Gagal Connect ke Server\n";
          sd_card_.appendFile(SD, "/log.txt", gagalServer.c_str());
        }

        else {

          Serial.println(" OK");
          String berhasilServer = "Berhasil Connect ke Server\n";
          sd_card_.appendFile(SD, "/log.txt", berhasilServer.c_str());
          Serial.println("Mengirim ke cloud...");
          delay(500);
          sendToServer();
          client2.stop();

          if (statekirim == 1) {
            if (!client2.connect(server2, port)) {
              Serial.println(" fail");
              statekirim = 2;
            } else {
              sendToServer4();
              client2.stop();
            }
          }

          if (statekirim == 2) {
            if (!client2.connect(server2, port)) {
              Serial.println(" fail");
              logSDCard();
            } else {
              sendToServer5();
              client2.stop();
          }
          }

          previous_time2 = millis();
          int counter2 = 1;
          int counter = 1;
          // myfile = SD.open("/datasensor.txt");
          // while(myfile.available()){
            while(counter){
            if ((millis() - previous_time2) < delay_offline) {
              int waktudelay = delay_offline - (millis() - previous_time2);
              delay(waktudelay);
            }

            cekfileada = 1;
            
            // buffer = myfile.readStringUntil('\n');
            Serial.println(buffer);
            
            if ((counter2 == 1) || (counter2 == 2)){
              if(!client2.connect(server4, port)){
                Serial.println("Tidak bisa connect lagi");
                buffer2 = buffer2 + buffer + '\n';
                gagal++;
              }
              else {
                berhasil++;
                Serial.print("Mengirim ");
                Serial.println(buffer);
                sendToServer2();
                client2.stop();
                previous_time2 = millis();
                buffer="";
             }
            } else {
              buffer3 = buffer3 + buffer + '\n';
            }
            
            counter2++;
            counter++;
             
          }
          
          // myfile.close();

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

          if (berhasil!=0){
            String berhasilConnect2 = "Berhasil Connect ke server lagi\n";
            sd_card_.appendFile(SD, "/log.txt", berhasilConnect2.c_str());
          }
          String hapus = "File telah dihapus\n";
          sd_card_.appendFile(SD, "/log.txt", hapus.c_str());
          modem.gprsDisconnect();
          Serial.println(F("GPRS disconnected"));
        }
      }
      aws1.print_all(result);
      
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
    quality = modem.getSignalQuality();
    logSDCard3();
    sd_card_.cekfile3();
    sd_card_.cekfile2();
    aws1.restartSensor(); //Matiin relay dan sensor
  }
  
  
}
