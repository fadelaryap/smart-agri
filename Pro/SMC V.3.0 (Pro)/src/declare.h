#define WDT_TIMEOUT 250

//-----Deklarasi SSID & AP Access Point-----//
const char *SSID_AP = "AWS Kedung Putri";
const char *PWD_AP = "12345678";
const char *SSID = "Modem Kedung Putri";
const char *PWD = "ptcp!2023";

//=--ApiKey, server, dan port---//
String keyyy = "";
String key = "";
const char* server = "http://api.smartagri.id";
const char* server3 = "http://agrieye.tp.ugm.ac.id";

uint8_t port = 80;    

int SSRPin = 13;
int buttonPin = 4;
int ledPin = 2;

int pengkali = 1000000;

bool maintenanceState;
float * result;

String tanggal, tanggal2, waktu;
int rssiPercentage, durasisleep;
float a_temp,a_humidity,a_atm_press,a_wind_speed,a_wind_dir,a_rainfall,a_radiance,a_pm25,a_pm10,a_voltage,a_power;
float c_temp,c_humidity,c_atm_press,c_wind_speed,c_wind_dir,c_rainfall,c_radiance,c_pm25,c_pm10,c_voltage,c_power;