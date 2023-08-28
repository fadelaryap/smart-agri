#ifndef aws.h
#define aws.h

#include <Arduino.h>
#include "modbus/ModbusMaster232.h"

#if defined(ESP8266)
    #include <Hash.h>
    #include <FS.h>
#elif defined(ESP32)
    #include <SPIFFS.h>
#endif

class Aws : public ModbusMaster232{
    public:
        Aws(uint8_t u8MBSlave, Stream &_serial) : ModbusMaster232(u8MBSlave, _serial){}
        // float get_temp();
        // float get_moist();
        int relayPin;
        float *get_all();
        float *get_all2();
        float *get_all3();
        float *get_all4();
        float *get_all5();
        float get_temp();
        float get_humidity();
        float get_atm_pressure();
        float get_wind_speed();
        float get_wind_dir();
        float get_rain();
        float get_radiance();
        float get_pm25();
        float get_pm10();
        void print_all(float []);
        void setupRelay(int _relayPin);
        void turnOnOFF(bool state);
        void hidupSensor();
        void restartSensor();
};

class NodeClient {
    public:
        NodeClient(String _serverURI, String _key);
        String serverURI;
        String key;
        String a1,a2,a3,a4,a5,a6,a7,a8,a9;
        String c1,c2,c3,c4,c5,c6,c7,c8,c9;
        String durasisleep, password;
        void sendData(float data[]);
        String readFile(fs::FS &fs, const char * path);
        String setApiFromFile();
        String setA_Sens1();
        String setA_Sens2();
        String setA_Sens3();
        String setA_Sens4();
        String setA_Sens5();
        String setA_Sens6();
        String setA_Sens7();
        String setA_Sens8();
        String setA_Sens9();
        String setC_Sens1();
        String setC_Sens2();
        String setC_Sens3();
        String setC_Sens4();
        String setC_Sens5();
        String setC_Sens6();
        String setC_Sens7();
        String setC_Sens8();
        String setC_Sens9();
        String durasi_sleep();
        String pass_word();
};


#endif