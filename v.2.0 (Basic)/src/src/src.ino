#include <Arduino.h>
#include <Wire.h>
// #include "awlms/awlms.h"
// #include "modbus/ModbusMaster232.h"
#include "WiFi.h"
#include <esp_task_wdt.h>
#include <ESPAsyncWebSrv.h>
// #include "mode_handler/ModeHandler.h"
#include <String.h>
#include <uRTCLib.h>
#include "FS.h"
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "../lib/ESP-DASH/src/ESPDash.h"
#include <Adafruit_INA219.h>
#include "DHT.h"


//Deklarasi RTC
uRTCLib rtc(0x68);
// char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Deklarasi address Gy-49
#define Address 0x4a

//Deklarasi Sensor DHT
#define DHTPIN 27
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

//Deklarasi sensor-sensor
Adafruit_INA219 ina219;

//Deklarasi nilai float
int status = 0;
float a,b,c,d,e,f, kelembaban, suhu;
float voltage, power, luminance, lux_smt;
float a_lux, a_KetinggianAir, a_SuhuC, a_Heat, a_Kelembaban;
float c_lux, c_KetinggianAir, c_SuhuC, c_Heat, c_Kelembaban;
float nilaiValue, h, t, hic, nilaiAwal, nilaiConv, nilaiAkhir, tinggi;
float nilai_awal, h_buff, t_buff, hic_buff, h_buffer, t_buffer, hic_buffer, h_bu, t_bu, hic_bu;

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
const char* SSID_AP ="ini punya widya"; //Site A
// const char* SSID_AP ="SMC Site B"; //Site B
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

#define WDT_TIMEOUT 120
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
String ASIK, WOW;
int pinmod = 34;
AW awl(pinmod, WOW);
NodeClient client(server3, key2);
ModeHandler mode(BUTTON_PIN,LED_PIN);
String key = "";
const char* server = "agrieye.tp.ugm.ac.id";
const char server2[] = "agrieye.tp.ugm.ac.id";
uint8_t port = 80;  

//--Start Webserver--//
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
  dataMessage = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(t) + "&C1=" + 
                String(h) + "&C2=" + String(luminance) + "&C3=" + String(nilaiValue) +
                "&C4=" + String(hic) + "&C13=" + String(voltage) + "&C14=" + 
                String(power) + "&R=3" + '\n';
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/datasensor.txt", dataMessage.c_str());
}

//--Menyimpan nilai sensor untuk websocket local web server--//
void logSDCard2() {
  char bif[20];
  
  dataMessage = String(t);
  Serial.print("Save data: ");

  Serial.println(dataMessage);
  writeFile(SD,"/SENS1.txt",dataMessage.c_str());

  String dataMessage5 = String(h);
  writeFile(SD,"/SENS2.txt",dataMessage5.c_str());

  String data_lux = String(luminance);
  writeFile(SD,"/SENS3.txt", data_lux.c_str());

  String data_KetinggianAir = String(nilaiValue);
  writeFile(SD,"/SENS4.txt", data_KetinggianAir.c_str());

  String data_HeatIndex = String(hic);
  writeFile(SD,"/SENS5.txt", data_HeatIndex.c_str());

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
  String requestData = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(t) + "&C1=" + 
                String(h) + "&C2=" + String(luminance) + "&C3=" + String(nilaiValue) 
                + "&C4=" + String(hic) + "&C13=" + String(voltage) + "&C14=" + String(power);
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
  String requestData = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(t) + "&C1=" + 
                String(h) + "&C2=" + String(luminance) + "&C3=" + String(nilaiValue) 
                + "&C4=" + String(hic) + "&C13=" + String(voltage) + "&C14=" + String(power) + "&R=1";
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
  String requestData = "&D=" + tanggal2 + "&T=" + waktu + "&C0=" + String(t) + "&C1=" + 
                String(h) + "&C2=" + String(luminance) + "&C3=" + String(nilaiValue) 
                + "&C4=" + String(hic) + "&C13=" + String(voltage) + "&C14=" + String(power) + "&R=2";
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
  String nilaisave = "Suhu Udara = " + String(t) + ", Kelembaban Udara = " + String(h) + ", Luminance = " + String(luminance) + ", Ketinggian Air = " + String(nilaiValue) + ", Heat Index = " + String(hic) + ", Voltage = "+ String(voltage) + ", Power = " + String(power) +'\n';
  return nilaisave;
}

//----------------Membaca config----------------//
void readConfig(){
  a11 = client.setA_Sens1(); a_lux = a11.toFloat();
  a22 = client.setA_Sens2(); a_KetinggianAir = a22.toFloat();
  a33 = client.setA_Sens3(); a_SuhuC = a33.toFloat();
  a44 = client.setA_Sens4(); a_Kelembaban = a44.toFloat();
  a55 = client.setA_Sens5(); a_Heat = a55.toFloat();
    
  c11 = client.setC_Sens1(); c_lux = c11.toFloat();
  c22 = client.setC_Sens2(); c_KetinggianAir = c22.toFloat();
  c33 = client.setC_Sens3(); c_SuhuC = c33.toFloat();
  c44 = client.setC_Sens4(); c_Kelembaban = c44.toFloat();
  c55 = client.setC_Sens5(); c_Heat = c55.toFloat();
  
  String buffer_tinggi = client.setTinggi(); tinggi = buffer_tinggi.toFloat();
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
  // int nilaiasik = 1;
  int waktusblm = millis();
  int waktuskrng = 0;
  for(address = 1; address < 127; address++ )
  {
    if(waktuskrng - waktusblm > 15000)  {
      ESP.restart();
    }
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    waktuskrng = millis();
    
    // Serial.println(nilaiasik);
    // nilaiasik++;
    if (error == 0)
    {
      Serial.print("I2C terbaca pada alamat 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      if(address == 74) {
        alamat = 74;
        String alamattt = String(alamat) + ", ";
        appendFile(SD, "/log.txt", alamattt.c_str());
      }
      else if(address == 75) {
        alamat = 75;
        String alamattt2 = String(alamat) + ", ";
        appendFile(SD, "/log.txt", alamattt2.c_str());
      }
      if(address == 64) {
        int alamat2 = 64;
        String alamattt3 = String(alamat2) + ", ";
        appendFile(SD, "/log.txt", alamattt3.c_str());
      }
      if(address == 104) {
        int alamat3 = 104;
        String alamattt4 = String(alamat3) + "\n";
        appendFile(SD, "/log.txt", alamattt4.c_str());
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
  String bangun = "Sudah bangun dari sleep\n";
  appendFile(SD, "/log.txt", bangun.c_str());
  Serial.println("SIM800L GPRS Test");
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX, MODEM_TX);
  
  URTCLIB_WIRE.begin();
  
  Serial.println("Maintanance button will be read in 5 seconds");
  delay(5000);
  mode.setupButton();
  maintananceState = mode.getMaintananceState(true);
  String bacai2c = "i2c yg terbaca: ";
  appendFile(SD, "/log.txt", bacai2c.c_str());
  alamat_gy = cariAddress();

  Wire.beginTransmission(alamat_gy);
  Wire.write(0x02);
  Wire.write(0x46);

  Wire.endTransmission();
  readConfig();
  analogReadResolution(12);
  awl.setupRelay(RELAY_PIN);
  if (maintananceState){
    mode.chooseWifiSetting(SSID_AP,PWD_AP);
    serverService.begin();
    awl.diedSensor(); 
    
  } 
  else {
    quality = modem.getSignalQuality();
    Serial.println(quality);
    modem.init();
    ina219.begin();
    // awl.hidupSensor();
    dht.begin();

    delay(5000);

    unsigned int data[2];
    String bacai2c = "i2c yg terbaca: ";
    appendFile(SD, "/log.txt", bacai2c.c_str());
    alamat_gy = cariAddress();
    Serial.println(alamat);
    // Wire.beginTransmission(alamat_gy); //start wire iic transmission
    // Wire.write(0x03); // Select data register
    // Wire.endTransmission(); // Stop iic transmission
    // Wire.requestFrom(alamat_gy, 1); // Request 2 bytes of data
    // // Read 2 bytes of data
    // // luminance msb, luminance lsb
    // if (Wire.available() == 1)
    // {
    //   data[0] = Wire.read();
    // }
    
    // Wire.beginTransmission(alamat_gy); //start wire iic transmission
    // Wire.write(0x04); // Select data register
    // Wire.endTransmission(); // Stop iic transmission
    // Wire.requestFrom(alamat_gy, 1); // Request 2 bytes of data
    // // Read 2 bytes of data
    // // luminance msb, luminance lsb
    // if (Wire.available() == 1)
    // {
    //   data[1] = Wire.read();
    // }

    // // Convert the data to lux
    // int exponent = (data[0] & 0xF0) >> 4;
    // int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
    
    // //float luminance = pow(2, exponent) * mantissa * 0.045;
    // // float luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
    // luminance = (float)(((0x0001 << exponent) * (float)mantissa) * 0.045);

    // luminance = (luminance * a_lux) + c_lux;
    // if(luminance < 0){
    //   luminance = 0;
    // }

    // nilaiAwal = analogRead(34);
    // nilaiConv = (nilaiAwal * 0.516) + 63.15;
    // nilaiAkhir = (nilaiConv * a_KetinggianAir) + c_KetinggianAir;
    // nilaiValue = tinggi - nilaiAkhir;

    // h_bu = dht.readHumidity(); 
    // t_bu = dht.readTemperature();
    // hic_bu = dht.computeHeatIndex(t, h, false);
    // h = (h_bu * a_Kelembaban) + c_Kelembaban;
    // t = (t_bu * a_SuhuC) + c_SuhuC;
    // hic = (hic_bu * a_Heat) + c_Heat;
    
    // voltage = ina219.getBusVoltage_V();
    // power = ina219.getPower_mW();

    // awl.diedSensor();

    // if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    //     Serial.println(" fail to Connect to GPRS");
    //   }

    // else {
    //   Serial.println(" OK");
    //   Serial.print("Connecting to ");
    //   Serial.println(server);

    //   if (!client2.connect(server2, port)) {
    //     Serial.println(" fail");
    //   }

    //   else {

    //     Serial.println(" OK");
    //     Serial.println("Mengirim ke cloud...");
    //     delay(500);
    //     sendToServer();
    //     client2.stop();
    //     modem.gprsDisconnect();
    //   }
    // }

    // delay(10000);
    Serial.println("mencoba connect");
    String msgg1 = "mencoba connect buat dapet waktu\n";
    appendFile(SD, "/log.txt", msgg1.c_str());
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      Serial.println(" fail");
      String msgg2 = "Gagal Connect\n";
      appendFile(SD, "/log.txt", msgg2.c_str());
    }
    else {
      Serial.println(" OK");
      String msgg3 = "Berhasil connect\n";
      appendFile(SD, "/log.txt", msgg3.c_str());
      Serial.println("Membaca waktu");
      String msgg4 = "Membaca Waktu\n";
      appendFile(SD, "/log.txt", msgg4.c_str());

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
        Serial.print("TAHUN: "); Serial.println(year3);
        int year4 = year3;
        year3 -= 2000;
        sec3 += 1;
        String msgg5 = "Waktu adalah = " + String(day3) + '-' + String(month3) + '-' + String(year3) + "  " + String(hour3) + ':' + String(min3) + ':' + String(sec3) + "\n";
        appendFile(SD, "/log.txt", msgg5.c_str());
        
        if((year3 != 4) && (year4 != 200)) {
          rtc.set(sec3,min3,hour3,1,day3,month3,year3);
        }
        
      }
      
      modem.gprsDisconnect();
      String msgg6 = "Disconnect GPRS\n";
      appendFile(SD, "/log.txt", msgg6.c_str());
      Serial.println(F("GPRS disconnected"));
    }

  }

  // nilai_awal = 0.00;
  // Serial.print("nilai H : "); Serial.println(tinggi); 
  // for(int il2=0;il2<50;il2++){
    
  //   nilaiAwal = analogRead(34);
    
  //   nilai_awal += nilaiAwal;
  //   Serial.print("Tinggi terbaca: "); Serial.print(nilaiAwal); Serial.print(", "); Serial.print("Nilai Total: ");
  //   Serial.println(nilai_awal);
  //   delay(100);
  // }

  
  // nilaiAwal = nilai_awal/50.00;
  // nilaiConv = (nilaiAwal * 0.516) + 63.15;
  // Serial.print("Nilai hasil konversi: "); Serial.println(nilaiConv);
  // nilaiAkhir = (nilaiConv * a_KetinggianAir) + c_KetinggianAir;
  // Serial.print("Nilai hasil kalibrasi: "); Serial.println(nilaiAkhir);

  // nilaiValue = tinggi - nilaiAkhir;
  // Serial.print("Nilai hasil kurang h: "); Serial.println(nilaiValue);
  // voltage = ina219.getBusVoltage_V();
  // power = ina219.getPower_mW();

  // Serial.print("voltage: "); Serial.print(voltage); Serial.print(", "); Serial.println(power);

  Serial.println("Connected ");
  Serial.println("Modbus RTU Master Online");
  

  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);

}

void loop() {
  esp_task_wdt_reset();
  
  
  if(!maintananceState){
    rtc.refresh();
    Serial.print(String(rtc.hour())); Serial.print(":"); Serial.print(String(rtc.minute())); Serial.print(":"); Serial.println(String(rtc.second()));
    
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
      appendFile(SD, "/log.txt", timestmp.c_str());
      String msg = "Masuk mode GPRS\n";
      appendFile(SD, "/log.txt", msg.c_str());

      awl.hidupSensor(); //tambahan
      String msg5 = "Sensor menyala\n";
      appendFile(SD, "/log.txt", msg5.c_str());
      delay(5000); //tambahan

      nilai_awal = 0.00;
      for(int il2=0;il2<50;il2++){
        
        nilaiAwal = analogRead(34);
        nilai_awal += nilaiAwal;
        delay(100);
      }

      
      nilaiAwal = nilai_awal/50.00;
      nilaiConv = (nilaiAwal * 0.516) + 63.15;
      nilaiAkhir = (nilaiConv * a_KetinggianAir) + c_KetinggianAir;
      
      nilaiValue = tinggi - nilaiAkhir;
      String nilaisave1 = "Tinggi Air: " + String(nilaiValue);
      appendFile(SD, "/log.txt", nilaisave1.c_str());


      h_buff = 0.00; t_buff=0.00; hic_buff=0.00;
      for(int il1=0;il1<9;il1++) {
        
        h_buffer = dht.readHumidity(); h_buff+=h_buffer;
        t_buffer = dht.readTemperature(); t_buff+=t_buffer;
        hic_buffer = dht.computeHeatIndex(t, h, false); hic_buff+=hic_buffer;
        delay(100);
        Serial.print(h_buffer);Serial.print(", ");Serial.print(t_buffer);Serial.print(", ");Serial.println(hic_buffer);
      }
      
      
      h_bu = h_buff/9.00; t_bu = t_buff/9.00; hic_bu = hic_buff/9.00;
      h = (h_bu * a_Kelembaban) + c_Kelembaban;
      t = (t_bu * a_SuhuC) + c_SuhuC;
      hic = (hic_bu * a_Heat) + c_Heat;
      
      if(hic<0){
        hic = 0;
      }
      String nilaisave5 = ", suhu: " + String(t) + ", kelembapan: " + String(h) + ", heat index: " + String(hic);
      appendFile(SD, "/log.txt", nilaisave5.c_str());

      voltage = ina219.getBusVoltage_V();
      power = ina219.getPower_mW();

      String nilaisave3 = "\nvoltage: " + String(voltage) + ", power: " + String(power) +"\n i2c yg terbaca: ";
      appendFile(SD, "/log.txt", nilaisave3.c_str());

      lux_smt = 0.00;
      for(int perul=0;perul<9;perul++){
        
        unsigned int data[2];
        alamat_gy = cariAddress();
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
        luminance = (float)(((0x00000001 << exponent) * (float)mantissa) * 0.045);
        lux_smt += luminance;
      }
      //=============GY - 49==========//
      
      luminance = lux_smt/9.00;
      luminance = (luminance * a_lux) + c_lux;
      if(luminance < 0){
        luminance = 0;
      }

      String nilaisave4 = ", Luminance: " + String(luminance) + "\n";
      appendFile(SD, "/log.txt", nilaisave4.c_str());

      //=======================================================//
      // delay(10000);

      

      awl.diedSensor(); 
      String msgg7 = "Relay Mati\n";
      appendFile(SD, "/log.txt", msgg7.c_str());

      // Serial.print("sensor air0 :");Serial.println(resultAir[0]);
      // Serial.print("sensor air1 :");Serial.println(resultAir[1]);
      Serial.print("suhu udara : ");Serial.println(t);
      Serial.print("kelembaban udara : ");Serial.println(h);
      Serial.print("lux : ");Serial.println(luminance);
      Serial.print("ketinggian air :");Serial.println(nilaiValue);
      Serial.print("heat index: ");Serial.println(hic);
      Serial.print("Tegangan: ");Serial.print(voltage);Serial.print("V | Daya: ");Serial.print(power);Serial.println(" mW");
      
      
      logSDCard2();
      
      String msgg8 = "mencoba connect untuk mengirim ke server\n";
      appendFile(SD, "/log.txt", msgg8.c_str());
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
      
      
      // esp_sleep_enable_timer_wakeup(pengkali * durasisleep);
      String mulaisleep = "Mulai sleep...\n\n";
      appendFile(SD, "/log.txt", mulaisleep.c_str());
      Serial.println("Going to sleep now");
      ESP.restart();
      // delay(1000);
      // Serial.flush(); 
      
      // delay(110000);
      // Serial.println("delay 1");
      // delay(110000);
      // Serial.println("delay 2");
      // delay(110000);
      // Serial.println("delay 3");
      // delay(30000);
      // esp_deep_sleep_start();

    }
  }
  else {
    quality = modem.getSignalQuality();
    logSDCard3();
    cekfile3();
    cekfile2();
    awl.diedSensor(); //Matiin relay dan sensor
  }
}