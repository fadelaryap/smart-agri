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


float buffer_rain;
float *result;
float *result1;
float *result3;
float *result4;
float *result5;

String smt1, smt2, smt3, smt4;

String buffer;
String terbaca;
String dataMessage;
String dataMessage3;
String waktu;
String tanggal,tanggal2;
String passwordAP,simpann,ccd2,cd2,buffer2,buffer3;
String keyyy="SmartAWSSapon";
String quicksend;
String payload;



String status;

int16_t quality;
int gagal = 0;
int berhasil = 0;
int cekfileada = 0;
int statekirim = 0;
float voltage, power;

unsigned int ArrayNilai[13];
int i=0;

char *maudikirim;

//--Deklarasi setting APN, gprsUser, gprsPASS, dan simPIN--//
const char apn[]      = "internet"; 
const char gprsUser[] = "";
const char gprsPass[] = "";
const char simPIN[]   = ""; 

//--Deklarasi nilai kalibrasi sensor--//
float a1,a2,a3,a4,a5,a6,a7,a8,a9;
float c1,c2,c3,c4,c5,c6,c7,c8,c9;
String a11,a22,a33,a44,a55,a66,a77,a88,a99;
String c11,c22,c33,c44,c55,c66,c77,c88,c99;
String server7 = "";String key2= "";
String myString, myString2, yy,mo,dd,hh,mm,ss;
int year, month, day, hour, minute, second, rssiPercentage;

const char* ntpServer = "pool.ntp.org";
const long gmtOffsetSec = 7 * 3600;  // GMT offset for Western Indonesia Time (WIB)
const int daylightOffsetSec = 0;

//-----Deklarasi SSID & AP Access Point-----//
const char *SSID = "UGM-Hotspot";
const char *PWD = "";