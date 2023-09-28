#include <Arduino.h>
#include <Wire.h>
#include "smc/smc.h"
#include "modbus/ModbusMaster232.h"
#include "WiFi.h"
#include <esp_task_wdt.h>
#include <ESPAsyncWebServer.h>
#include "mode_handler/ModeHandler.h"
#include <String.h>
#include <uRTCLib.h>
#include "FS.h"
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <ESPDash.h>
#include <Adafruit_INA219.h>

//Deklarasi RTC
uRTCLib rtc(0x68);
// char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Deklarasi address Gy-49
#define Address 0x4a

//Deklarasi sensor-sensor
AirhumSensor air1(1, Serial2);
SoilSensor smc1(2, Serial2);
SoilSensor smc2(3, Serial2);
SoilSensor smc3(4, Serial2);
NPKSensor sensorNPK(5, Serial2);
Adafruit_INA219 ina219;

//Deklarasi nilai float
int status = 0;
float *resultSoil1,*resultSoil2,*resultSoil3,*resultAir,*resultNPK;
float *resultPack;
float a,b,c,d,e,f, kelembaban, suhu;
float voltage, power, luminance, lux_smt;
float a_Tudara,a_Hudara,a_Lux,a_Lengas1,a_Lengas2,a_Lengas3,a_Ttanah1,a_Ttanah2,a_Ttanah3, a_pH,a_Nitrogen, a_Pho, a_Kalium;
float c_Tudara,c_Hudara,c_Lux,c_Lengas1,c_Lengas2,c_Lengas3,c_Ttanah1,c_Ttanah2,c_Ttanah3, c_pH, c_Nitrogen, c_Pho, c_Kalium;

//Deklarasi nilai String
String a11,a22,a33,a44,a55,a66,a77,a88,a99,a1010,a1111,a1212,a1313;
String c11,c22,c33,c44,c55,c66,c77,c88,c99,c1010,c1111,c1212,c1313;
String server3 = "";String key2= "";
String myString, myString2, yy,mo,dd,hh,mm,ss;
String buffer, endPoint;
String terbaca;
String dataMessage;
String dataMessage3;
String waktu;
String tanggal,tanggal2;
String passwordAP,simpann,ccd2,cd2,buffer2,buffer3; 
String smt1, smt2, smt3, smt4;

//Deklarasi
// const char* SSID_AP ="SMC Site A"; //Site A
const char* SSID_AP ="SMC Site B"; //Site B
// const char* SSID_AP ="SMC Site C"; //Site C
const char* PWD_AP = "smartagri22";

int nilaii1, nilaii2, nilaii3, nilaii4, nilaii5, alamat_gy;
int nCek1, nCek2, nCek3, nCek4, nCek5, nCek6, nCek7, nCek8, nCek9, nCek10, nCek11, nCek12, nCek13, nCek14;


// String server = "http://agrieye.tp.ugm.ac.id/feed/api2.php";
// String key = "befab05b240b972098ac1beb0caa52d1";//smc board 1
// String key = "81035e1df471724de7420da81bd1767d";//smc board 2
// String key = "305e44f10a183c7c96c81402084d3ccd";//smc board 3
// String server = " ";
// String key = "87bb155481f2d8308cff4a2e247b5ce5z";
// String key = " ";

SoftwareSerial SIM800L(32,33);

const char apn[]      = "internet"; 
const char gprsUser[] = "";
const char gprsPass[] = "";
const char simPIN[]   = ""; 

#define WDT_TIMEOUT 250
#define BUTTON_PIN 4
#define LED_PIN 2
#define RELAY_PIN 13
#define MODEM_TX 33
#define MODEM_RX 32
#define MODEM_RST 25
#define SerialAT Serial1

//--Set type to SIM800--//
#define TINY_GSM_MODEM_SIM800 
#define TINY_GSM_RX_BUFFER   1024

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

TinyGsmClient client2(modem);

NodeClient client(server3, key2);
ModeHandler mode(BUTTON_PIN,LED_PIN);
String key = "";
const char* server = "agrieye.tp.ugm.ac.id";
const char server2[] = "agrieye.tp.ugm.ac.id";
uint8_t port = 80;  

//--Start Webserver--///
AsyncWebServer serverService(80);


/* Attach ESP-DASH to AsyncWebServer */
ESPDash dashboard(&serverService, true);

float *result;
bool is_first = true;

int16_t quality;
int gagal = 0;
int berhasil = 0;
int cekfileada = 0;
int statekirim = 0;

File myfile;

//--Deklarasi nilai status maintanance--//
bool maintananceState = false;

//---Status Sinyal sebagai penentu kirim ke server---//
bool kirimcloud = false;


int delay_time = 2000;
int delay_offline = 15000;
int sendDelay = 0.5 * 60 * 1000;
int previous_time = 0;
int alamat = 0;
int previous_time2 = 0;
int statusGPRS;



void initSDCard(){
   if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);
    File file = fs.open(path, "w");
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  myfile = fs.open(path, FILE_APPEND);
  if(!myfile) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(myfile.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  myfile.close();
}

void cekfile(){
  myfile = SD.open("/datasensor.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/datasensor.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
}

void ceksleepdanpassword(){
  myfile = SD.open("/durasi_sleep.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/durasi_sleep.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
  myfile.close();
  delay(1000);
  myfile = SD.open("/pass_word.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/pass_word.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
  myfile.close();
  delay(1000);
}


void cekfile2(){
  for(int j = 1;j<16;j++) {
    char baf[20];
    sprintf(baf, "/SENS%d.txt",j);
    myfile = SD.open(baf);
    if(!myfile) {
      Serial.println("File doesn't exist");
      Serial.println("Creating file...");
      myfile = SD.open(baf,FILE_WRITE);
    }
    else {
      Serial.println("File already exists");  
    }
  }
  myfile.close();
}

void cekfile3(){
  myfile = SD.open("/RSSI.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/RSSI.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
  
  myfile.close();
}

//belum diubah
void logSDCard() {
  dataMessage = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(resultAir[3]) + "&C1=" + 
                String(resultAir[2]) + "&C2=" + String(luminance) + "&C3=" + String(resultSoil1[3]) 
                + "&C4=" + String(resultSoil1[2]) + "&C5=" + String(resultSoil2[3]) + "&C6=" 
                + String(resultSoil2[2]) + "&C7=" + String(resultSoil3[3]) + "&C8=" 
                + String(resultSoil3[2]) + "&C9=" + String(resultNPK[2]) + "&C10=" + String(resultNPK[3]) + "&C11=" + String(resultNPK[4]) + "&C12=" + String(resultNPK[5]) + "&C13=" + String(voltage) + "&C14=" + String(power) + "&R=3" + '\n';
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/datasensor.txt", dataMessage.c_str());
}



//--Menyimpan nilai sensor untuk websocket local web server--//
void logSDCard2() {
  char bif[20];
  
  dataMessage = String(resultAir[3]);
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  writeFile(SD,"/SENS1.txt",dataMessage.c_str());

  String dataMessage5 = String(resultAir[2]);
  writeFile(SD,"/SENS2.txt",dataMessage5.c_str());

  String data_lux = String(luminance);
  writeFile(SD,"/SENS3.txt", data_lux.c_str());

  String data_LengasSoil1 = String(resultSoil1[3]);
  writeFile(SD,"/SENS4.txt", data_LengasSoil1.c_str());

  String data_SuhuSoil1 = String(resultSoil1[2]);
  writeFile(SD,"/SENS5.txt", data_SuhuSoil1.c_str());

  String data_LengasSoil2 = String(resultSoil2[3]);
  writeFile(SD,"/SENS6.txt", data_LengasSoil2.c_str());

  String data_SuhuSoil2 = String(resultSoil2[2]);
  writeFile(SD,"/SENS7.txt", data_SuhuSoil2.c_str());

  String data_LengasSoil3 = String(resultSoil3[3]);
  writeFile(SD,"/SENS8.txt", data_LengasSoil3.c_str());

  String data_SuhuSoil3 = String(resultSoil3[2]);
  writeFile(SD,"/SENS9.txt", data_SuhuSoil3.c_str());

  char baf[20];
  for(int l=10;l<14;l++){
    sprintf(bif, "/SENS%d.txt",l);
    int ll = l - 8;
    String data_NPK = String(resultNPK[ll]);
    Serial.print("Save data: ");
    Serial.println(data_NPK);
    writeFile(SD,bif,data_NPK.c_str());
    delay(100);
    // appendFile(SD, bif, dataMessage.c_str());
  }

  String data_voltage = String(voltage);
  String data_power = String(power);

  writeFile(SD,"/VOLTAGE.txt", data_voltage.c_str());
  writeFile(SD,"/DAYA.txt", data_power.c_str());


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
  writeFile(SD, "/RSSI.txt",dataMessage3.c_str());
  delay(1000);
    // appendFile(SD, bif, dataMessage.c_str());
}

void sendToServer2() {
  Serial.println("Performing HTTP POST request...");
  Serial.println(buffer);
  client2.print("GET ");
  client2.print(endPoint);
  client2.print("?K=");
  client2.print(key);
  client2.print(buffer);
  client2.print(" HTTP/1.0\n");
  client2.print("Host: ");
  client2.println(server);
  client2.print("User-Agent: Arduino-agrieye\n");
  client2.println();
  unsigned long timeout = millis();
  while (client2.connected() && millis() - timeout < 10000L) {
    // Print available data (HTTP response from server)
    while (client2.available()) {
      String cd = client2.readStringUntil('\n');
      cd2 += cd;
      Serial.println(cd);
      timeout = millis();
      // appendFile(SD, "/log.txt", cd.c_str());
    }
  }
  
  if (cd2.indexOf('#') == -1) {
    buffer2 = buffer2 + buffer + '\n';
    Serial.println("Data tidak terkirim");
  }
  else {
    Serial.println("Data dari file terkirim nih");
  }
}

void sendToServer() {
  String requestData = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(resultAir[3]) + "&C1=" + 
                String(resultAir[2]) + "&C2=" + String(luminance) + "&C3=" + String(resultSoil1[3]) 
                + "&C4=" + String(resultSoil1[2]) + "&C5=" + String(resultSoil2[3]) + "&C6=" 
                + String(resultSoil2[2]) + "&C7=" + String(resultSoil3[3]) + "&C8=" 
                + String(resultSoil3[2]) + "&C9=" + String(resultNPK[2]) + "&C10=" + String(resultNPK[3]) + "&C11=" + String(resultNPK[4]) + "&C12=" + String(resultNPK[5]) + "&C13=" + String(voltage) + "&C14=" + String(power);
  Serial.println(requestData); 
  Serial.println("Performing HTTP POST request...");
  // client2.print("GET /feed/api2.php?K=");
  client2.print("GET ");
  client2.print(endPoint);
  client2.print("?K=");
  // String buatkey = "/feed/api2.php?K=" + String(key) + String(requestData);
  // client2.print(String("GET ") + buatkey + " HTTP/1.0\r\n");
  client2.print(key);
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
      if (simpann.indexOf('#')== -1){
        nilaii3 = 1;
      } else {
        nilaii3 = 2;
      }
      ccd2 += ccd;
      timeout = millis();
      
    }
  }
  appendFile(SD, "/log.txt", ccd2.c_str());
  if (nilaii3 == 2){
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  } 
  else if(nilaii3 == 1){
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server\n";
    appendFile(SD, "/log.txt", gagalmeng.c_str());
    statekirim = 1;
  }
}

void sendToServer4() {
  String requestData = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(resultAir[3]) + "&C1=" + 
                String(resultAir[2]) + "&C2=" + String(luminance) + "&C3=" + String(resultSoil1[3]) 
                + "&C4=" + String(resultSoil1[2]) + "&C5=" + String(resultSoil2[3]) + "&C6=" 
                + String(resultSoil2[2]) + "&C7=" + String(resultSoil3[3]) + "&C8=" 
                + String(resultSoil3[2]) + "&C9=" + String(resultNPK[2]) + "&C10=" + String(resultNPK[3]) + "&C11=" + String(resultNPK[4]) + "&C12=" + String(resultNPK[5]) + "&C13=" + String(voltage) + "&C14=" + String(power) + "&R=1";
  Serial.println(requestData); 
  Serial.println("Performing HTTP POST request...");
  // client2.print("GET /feed/api2.php?K=");
  client2.print("GET ");
  client2.print(endPoint);
  client2.print("?K=");
  // String buatkey = "/feed/api2.php?K=" + String(key) + String(requestData);
  // client2.print(String("GET ") + buatkey + " HTTP/1.0\r\n");
  client2.print(key);
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
      
      if (ccd2.indexOf('#') == -1) {
        nilaii4 = 1;
      }
      else {
        nilaii4 = 2;
      }
    }
  }

  appendFile(SD, "/log.txt", ccd2.c_str());
  if (nilaii4 == 1) {
    statekirim = 2;
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server lagi\n";
    appendFile(SD, "/log.txt", gagalmeng.c_str());
  }
  else if(nilaii4 == 2) {
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  }
}

void sendToServer5() {
  String requestData = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(resultAir[3]) + "&C1=" + 
                String(resultAir[2]) + "&C2=" + String(luminance) + "&C3=" + String(resultSoil1[3]) 
                + "&C4=" + String(resultSoil1[2]) + "&C5=" + String(resultSoil2[3]) + "&C6=" 
                + String(resultSoil2[2]) + "&C7=" + String(resultSoil3[3]) + "&C8=" 
                + String(resultSoil3[2]) + "&C9=" + String(resultNPK[2]) + "&C10=" + String(resultNPK[3]) + "&C11=" + String(resultNPK[4]) + "&C12=" + String(resultNPK[5]) + "&C13=" + String(voltage) + "&C14=" + String(power) +"&R=2";
  Serial.println(requestData); 
  Serial.println("Performing HTTP POST request...");
  // client2.print("GET /feed/api2.php?K=");
  client2.print("GET ");
  client2.print(endPoint);
  client2.print("?K=");
  // String buatkey = "/feed/api2.php?K=" + String(key) + String(requestData);
  // client2.print(String("GET ") + buatkey + " HTTP/1.0\r\n");
  client2.print(key);
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
      
      if (ccd2.indexOf('#') == -1) {
        nilaii5 = 1;
      }
      else {
        nilaii5 = 2;
      }
    }
  }

  appendFile(SD, "/log.txt", ccd2.c_str());

  if (nilaii5 == 1) {
    Serial.println("Gagal mengirim");
    String gagalmeng = "Gagal mengirim ke server lagi lagi\n";
    appendFile(SD, "/log.txt", gagalmeng.c_str());
    logSDCard();
  }
  else if(nilaii5 == 2){
    Serial.println("Data terkirim nih");
    String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
    appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
  }
}

String print() {
  String nilaisave = "Suhu Udara = " + String(resultAir[3]) + ", Kelembaban Udara = " + String(resultAir[2]) + ", Luminance = " + String(luminance) + ", Lengas Tanah 1 = " + String(resultSoil1[3]) + ", Suhu Tanah 1 = " + String(resultSoil1[2]) + ", Lengas Tanah 2 = " + String(resultSoil2[3]) + ", Suhu Tanah 2 = " + String(resultSoil2[2]) + ", Lengas Tanah 3 = " + String(resultSoil3[3]) + ", Suhu Tanah 3 = " + String(resultSoil3[2]) + ", pH = " + String(resultNPK[2]) + ", Nitrogen = " + String(resultNPK[3]) + ", Phospor = " + String(resultNPK[4]) + ", Kalium = " + String(resultNPK[5]) + '\n';
  return nilaisave;
}

//----------------Membaca config----------------//
void readConfig(){
  a11 = client.setA_Sens1(); a_Tudara = a11.toFloat();
  a22 = client.setA_Sens2(); a_Hudara = a22.toFloat();
  a33 = client.setA_Sens3(); a_Lux = a33.toFloat();
  a44 = client.setA_Sens4(); a_Lengas1 = a44.toFloat();
  a55 = client.setA_Sens5(); a_Ttanah1 = a55.toFloat();
  a66 = client.setA_Sens6(); a_Lengas2 = a66.toFloat();
  a77 = client.setA_Sens7(); a_Ttanah2 = a77.toFloat();
  a88 = client.setA_Sens8(); a_Lengas3 = a88.toFloat();
  a99 = client.setA_Sens9(); a_Ttanah3 = a99.toFloat();
  a1010 = client.setA_Sens10(); a_pH = a1010.toFloat();
  a1111 = client.setA_Sens11(); a_Nitrogen = a1111.toFloat();
  a1212 = client.setA_Sens12(); a_Pho = a1212.toFloat();
  a1313 = client.setA_Sens13(); a_Kalium = a1313.toFloat();
  
  c11 = client.setC_Sens1(); c_Tudara = c11.toFloat();
  c22 = client.setC_Sens2(); c_Hudara = c22.toFloat();
  c33 = client.setC_Sens3(); c_Lux = c33.toFloat();
  c44 = client.setC_Sens4(); c_Lengas1 = c44.toFloat();
  c55 = client.setC_Sens5(); c_Ttanah1 = c55.toFloat();
  c66 = client.setC_Sens6(); c_Lengas2 = c66.toFloat();
  c77 = client.setC_Sens7(); c_Ttanah2 = c77.toFloat();
  c88 = client.setC_Sens8(); c_Lengas3 = c88.toFloat();
  c99 = client.setC_Sens9(); c_Ttanah3 = c99.toFloat();
  c1010 = client.setC_Sens10(); c_pH = c1010.toFloat();
  c1111 = client.setC_Sens11(); c_Nitrogen = c1111.toFloat();
  c1212 = client.setC_Sens12(); c_Pho = c1212.toFloat();
  c1313 = client.setC_Sens13(); c_Kalium = c1313.toFloat();
  String durasi = client.durasi_sleep(); durasisleep = durasi.toInt();
  key = client.setApiFromFile(); 
  endPoint = client.setEndPoint();
}

int cariAddress(){
  int alamat;

  byte error, address;
  int nDevices; 
  Serial.println("Memindai...");
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
  
    if (error == 0)
    {
      Serial.print("I2C terbaca pada alamat 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      if(address == 74) {
        alamat = 74;
      }
      else if(address == 75) {
        alamat = 75;
      }
      
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Ada error yang tidak diketahui pada alamat 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }

  if (nDevices == 0)
    Serial.println("Tidak ada satupun alamat I2C yang ditemukan\n");
  else
    Serial.println("selesai\n");

  return alamat;
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  Serial2.begin(9600);
 
  
  SIM800L.begin(9600);

  Serial.println("SD Card Test");
  
  initSDCard();

  myfile.close();    
  Serial.println("SIM800L GPRS Test");
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  
  URTCLIB_WIRE.begin();
  
  Serial.println("Maintanance button will be read in 5 seconds");
  delay(5000);
  mode.setupButton();
  maintananceState = mode.getMaintananceState(true);

  alamat_gy = cariAddress();

  Wire.beginTransmission(alamat_gy);
  Wire.write(0x02);
  Wire.write(0x46);

  Wire.endTransmission();
  readConfig();

  smc1.setupRelay(RELAY_PIN);
  if (maintananceState){
    mode.chooseWifiSetting(SSID_AP,PWD_AP);
    serverService.begin();
    smc1.diedSensor(); 
  } 
  else {
    modem.init();
    ina219.begin();
    smc1.begin();
    smc2.begin();
    smc3.begin();
    air1.begin();
    sensorNPK.begin();
    smc1.hidupSensor();
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
        year3 -= 2000;
        sec3 += 1;
        rtc.set(sec3,min3,hour3,1,day3,month3,year3);
      }

    //   // client2.print("GET /feed/api2.php?K=");
    //   client2.print("GET ");
    //   client2.print(/feed/api-config.php);
    //   client2.print("?K=");
    //   // String buatkey = "/feed/api2.php?K=" + String(key) + String(requestData);
    //   // client2.print(String("GET ") + buatkey + " HTTP/1.0\r\n");
    //   client2.print(key);
    //   client2.print(" HTTP/1.0\n");
    //   // client2.print(String("Host: ") + server + "\r\n");
    //   client2.print("Host: ");
    //   client2.println(server);
    //   // String useragent = "Arduino-agrieye";
    //   // client2.print(String("User-Agent: ") + useragent + "\r\n");
    //   // client2.print("Connection: close\r\n\r\n");
    //   client2.print("User-Agent: Arduino-agrieye\n");
    //   client2.println();

    //   unsigned long timeout = millis();
    //   while (client2.connected() && millis() - timeout < 20000L) {
    //     // Print available data (HTTP response from server)
    //     while (client2.available()) {
    //       char ccd = client2.read();
    //       Serial.println(ccd);
    //       smt1 = String(ccd);
    //       simpann += ccd;
    //       String asik = simpann.readStringUntil('#');
          
    //       if (simpann.indexOf('#')== -1){
    //         nilaii3 = 1;
    //       } else {
    //         nilaii3 = 2;
    //       }
    //       ccd2 += ccd;
    //       timeout = millis();
          
    //     }
    //   }

      modem.gprsDisconnect();
      Serial.println(F("GPRS disconnected"));
    }

  }

  Serial.println("Connected ");
  Serial.println("Modbus RTU Master Online");
  

  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

}

void loop() {
  esp_task_wdt_reset();
  
  
  if(!maintananceState){
    rtc.refresh();
    // Serial.print(String(rtc.hour())); Serial.print(":"); Serial.print(String(rtc.minute())); Serial.print(":"); Serial.println(String(rtc.second()));
    
    // uint8_t bagi = rtc.minute() % 10;
    // uint8_t detik2 = rtc.second();

    
    // if((bagi != 0) && (detik2 != 0)){
    //   delay(1000);
    //   return;
    // }
    // else if ((bagi == 0) && (detik2 == 0)){
      tanggal = String(rtc.year())+ '-' +String(rtc.month())+'-'+String(rtc.day());
      tanggal2 = "20" + String(rtc.year())+ '-' +String(rtc.month())+'-'+String(rtc.day());
      waktu = String(rtc.hour())+':'+String(rtc.minute())+':'+String(rtc.second());
      String timestmp = tanggal2 + ' ' + waktu + "\n";
      appendFile(SD, "/log.txt", timestmp.c_str());
      String msg = "Masuk mode GPRS\n";
      appendFile(SD, "/log.txt", msg.c_str());
      
      for(int perul=0;perul<9;perul++){
        lux_smt = 0.00;
        unsigned int data[2];
        // Serial.println(alamat);
        Wire.beginTransmission(alamat_gy); //start wire iic transmission
        Wire.write(0x03); // Select data register
        Wire.endTransmission(); // Stop iic transmission
        Wire.requestFrom(alamat_gy, 1); // Request 2 bytes of data
        // Read 2 bytes of data
        // luminance msb, luminance lsb
        if (Wire.available() == 1)
        {
          data[0] = Wire.read();
        }

        Wire.beginTransmission(alamat_gy); //start wire iic transmission
        Wire.write(0x04); // Select data register
        Wire.endTransmission(); // Stop iic transmission
        Wire.requestFrom(alamat_gy, 1); // Request 2 bytes of data
        // Read 2 bytes of data
        // luminance msb, luminance lsb
        if (Wire.available() == 1)
        {
          data[1] = Wire.read();
        }

        // Convert the data to lux
        int exponent = (data[0] & 0xF0) >> 4;
        int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
        
        //float luminance = pow(2, exponent) * mantissa * 0.045;
        // float luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
        luminance = (float)(((0x0001 << exponent) * (float)mantissa) * 0.045);
        lux_smt += luminance;
      }
      //=============GY - 49==========//
      
      luminance = lux_smt/9.00;
      
      //=======================================================//
      // delay(10000);
      resultAir = air1.get_Airhumidty();
      delay(200);

      //Kalkulasi data dengan nilai kalibrasi
      resultAir[2] = (a_Hudara * resultAir[2]) + c_Hudara;
      resultAir[3] = (a_Tudara * resultAir[3]) + c_Tudara;

      resultSoil1 = smc1.get_all();
      resultSoil1[2] = resultSoil1[2];
      resultSoil1[3] = resultSoil1[3];
      delay(200);

      nCek1 = resultSoil1[2] * 1;
      nCek2 = resultSoil1[3] * 1;
      Serial.println(nCek1);Serial.print(", ");Serial.println(nCek2);

      if(nCek1 != 0) {
        resultSoil1[2] = (a_Ttanah1 * resultSoil1[2]) + c_Ttanah1;
      }
      
      if(nCek2 != 0) {
        resultSoil1[3] = (a_Lengas1 * resultSoil1[3]) + c_Lengas1;
      }
      
      resultSoil2 = smc2.get_all();

      resultSoil2[2] = resultSoil2[2];
      resultSoil2[3] = resultSoil2[3];
      delay(200);

      nCek3 = resultSoil2[2] * 1;
      nCek4 = resultSoil2[3] * 1;
      Serial.print(nCek3);Serial.print(", ");Serial.println(nCek4);

      if(nCek3 != 0) {
        resultSoil2[2] = (a_Ttanah2 * resultSoil2[2]) + c_Ttanah2;
      }

      if(nCek4 != 0){
        resultSoil2[3] = (a_Lengas2 * resultSoil2[3]) + c_Lengas2;
      }
      
      resultSoil3 = smc3.get_all();
      resultSoil3[2] = resultSoil3[2];
      resultSoil3[3] = resultSoil3[3];
      delay(200);

      nCek5 = resultSoil3[2] * 1;
      nCek6 = resultSoil3[3] * 1;
      Serial.print(nCek5);Serial.print(", ");Serial.println(nCek6);

      if(nCek5 != 0) {
        resultSoil3[2] = (a_Ttanah3 * resultSoil3[2]) + c_Ttanah3;
      }

      if(nCek6 != 0){
        resultSoil3[3] = (a_Lengas3 * resultSoil3[3]) + c_Lengas3;
      }
      
      resultNPK = sensorNPK.get_NPKSensor();
      resultNPK[2] = resultNPK[2];
      resultNPK[3] = resultNPK[3];
      resultNPK[4] = resultNPK[4];
      resultNPK[5] = resultNPK[5];
      
      nCek7 = resultNPK[2] * 1;
      nCek8 = resultNPK[3] * 1;
      nCek9 = resultNPK[4] * 1;
      nCek10 = resultNPK[5] * 1;
      Serial.print(nCek7);Serial.print(", ");Serial.print(nCek8);Serial.print(", ");Serial.print(nCek9);Serial.print(", ");Serial.println(nCek10);

      if(nCek7 != 0) {
        resultNPK[2] = (a_pH * resultNPK[2]) + c_pH;
      }
      
      if(nCek8 != 0) {
        resultNPK[3] = (a_Nitrogen * resultNPK[3]) + c_Nitrogen;
      }
      
      if(nCek9 != 0) {
        resultNPK[4] = (a_Pho * resultNPK[4]) + c_Pho;
      }
      
      if(nCek10 != 0){
        resultNPK[5] = (a_Kalium * resultNPK[5]) + c_Kalium;
      }
      

      voltage = ina219.getBusVoltage_V();
      power = ina219.getPower_mW();

    
      // Serial.print("sensor air0 :");Serial.println(resultAir[0]);
      // Serial.print("sensor air1 :");Serial.println(resultAir[1]);
      Serial.print("suhu udara : ");Serial.println(resultAir[3]);
      Serial.print("kelembaban udara : ");Serial.println(resultAir[2]);
      Serial.print("lux : ");Serial.println(luminance);
      Serial.print("lengas tanah 1 :");Serial.println(resultSoil1[3]);
      Serial.print("suhu tanah 1 : ");Serial.println(resultSoil1[2]);
      Serial.print("lengas tanah 2 :");Serial.println(resultSoil2[3]);
      Serial.print("suhu tanah 2 : ");Serial.println(resultSoil2[2]);
      Serial.print("lengas tanah 3 :");Serial.println(resultSoil3[3]);
      Serial.print("suhu tanah 3 : ");Serial.println(resultSoil3[2]);
      Serial.print("pH : ");Serial.println(resultNPK[2]);
      Serial.print("Nitrogen : ");Serial.println(resultNPK[3]);
      Serial.print("Phosporus : ");Serial.println(resultNPK[4]);
      Serial.print("Potasium/Kalium : ");Serial.println(resultNPK[5]);
      Serial.print("Tegangan: ");Serial.print(voltage);Serial.print("V | Daya: ");Serial.print(power);Serial.println(" mW");
      
      smc1.diedSensor(); 
      String nilaisave2 = print();
      appendFile(SD, "/log.txt", nilaisave2.c_str());
            
      logSDCard2();
      
      Serial.println("Mencoba Connect");
      Serial.println(key);

      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" fail to Connect to GPRS");
        cekfile();
        logSDCard();
        String gagalconnect = "Gagal Connect ke GRPS\n";
        appendFile(SD, "/log.txt", gagalconnect.c_str());
      }

      else {
        Serial.println(" OK");
        String berhasilGPRS = "Berhasil tersambung dengan GPRS\n";
        appendFile(SD, "/log.txt", berhasilGPRS.c_str());
        Serial.print("Connecting to ");
        Serial.println(server);

        if (!client2.connect(server2, port)) {
          Serial.println(" fail");
          logSDCard();
          String gagalServer = "Gagal Connect ke Server\n";
          appendFile(SD, "/log.txt", gagalServer.c_str());
        }

        else {

          Serial.println(" OK");
          String berhasilServer = "Berhasil Connect ke Server\n";
          appendFile(SD, "/log.txt", berhasilServer.c_str());
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
          myfile = SD.open("/datasensor.txt");
          while(myfile.available()){
            
            if ((millis() - previous_time2) < delay_offline) {
              int waktudelay = delay_offline - (millis() - previous_time2);
              delay(waktudelay);
            }

            cekfileada = 1;
            
            buffer = myfile.readStringUntil('\n');
            Serial.println(buffer);
            
            if ((counter2 == 1) || (counter2 == 2)){
              if(!client2.connect(server2, port)){
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
          
          myfile.close();

          SD.remove("/datasensor.txt");
          
          if((buffer3=="") && (buffer2!="")) {
            writeFile(SD, "/datasensor.txt", buffer2.c_str());
          } 
          else if ((buffer2=="") && (buffer3!="")) {
            writeFile(SD, "/datasensor.txt", buffer3.c_str());
          }
          else if ((buffer3 != "") && (buffer2 != "")){
            writeFile(SD, "/datasensor.txt", buffer3.c_str());
            appendFile(SD, "/datasensor.txt", buffer2.c_str());
          }
          
          if (gagal != 0) {
              String gagalConnect2 = "Gagal mengirim lagi sebanyak " + String(gagal) + " lagi\n";
              appendFile(SD, "/log.txt", gagalConnect2.c_str());
          }

          if (cekfileada == 0){
            String adafile = "Masih ada file blm terkirim\n";
            appendFile(SD, "/log.txt", adafile.c_str());
          }

          if (berhasil!=0){
            String berhasilConnect2 = "Berhasil Connect ke server lagi\n";
            appendFile(SD, "/log.txt", berhasilConnect2.c_str());
          }
          String hapus = "File telah dihapus\n";
          appendFile(SD, "/log.txt", hapus.c_str());
          modem.gprsDisconnect();
          Serial.println(F("GPRS disconnected"));
        }
      }
      
      
      esp_sleep_enable_timer_wakeup(pengkali * durasisleep);
      String mulaisleep = "Mulai sleep...\n\n";
      appendFile(SD, "/log.txt", mulaisleep.c_str());
      Serial.println("Going to sleep now");
      delay(1000);
      Serial.flush(); 
      esp_deep_sleep_start();

    // }
  }
  else {
    quality = modem.getSignalQuality();
    logSDCard3();
    cekfile3();
    cekfile2();
    smc1.diedSensor(); //Matiin relay dan sensor
  }
}