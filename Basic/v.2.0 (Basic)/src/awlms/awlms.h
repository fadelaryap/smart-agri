#ifndef awlms.h
#define awlms.h

#include <Arduino.h>
#include "modbus/ModbusMaster232.h"
#include <SPIFFS.h>

class AW {
    public:
        AW(int ASIK, String WOW);
        int relayPin;
        String nilai_c;
        String nilai_f;
        int PIN;
        void setupRelay(int _relayPin);
        void turnOnOFF(bool state);
        void hidupSensor();
        float bacaData();
        void diedSensor();
        
};

class NodeClient {
    public:
        NodeClient(String _serverURI, String _key);
        String serverURI;
        String key;
        String a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16;
        String c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16;
        String durasisleep, password, idd_data;
        String endPoint;
        void sendData(float soilRes[]);
        float *packSmcData(float soil1[],float soil2[], float soil3[], float airHum1[], float lux);
        String readFile(fs::FS &fs, const char * path);
        String setEndPoint();
        String setApiFromFile();
        String setA_Sens1();
        String setA_Sens2();
        String setTinggi();
        String setA_Sens3();
        String setA_Sens4();
        String setA_Sens5();
        String setC_Sens1();
        String setC_Sens2();
        String setC_Sens3();
        String setC_Sens4();
        String setC_Sens5();
        String durasi_sleep();
        String pass_word();
        String readID();
};

#endif