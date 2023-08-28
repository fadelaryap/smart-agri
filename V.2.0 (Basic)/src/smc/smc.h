#ifndef smc.h
#define smc.h

#include <Arduino.h>
#include "modbus/ModbusMaster232.h"
#include <SPIFFS.h>


class SoilSensor : public ModbusMaster232{
    public:
        SoilSensor(uint8_t u8MBSlave, Stream &_serial) : ModbusMaster232(u8MBSlave, _serial){}
        int relayPin;
        float get_temp();
        float get_moist();
        float *get_all();
        void setupRelay(int _relayPin);
        void turnOnOFF(bool state);
        void hidupSensor();
        void diedSensor();

        static const int result_size = 4;
        float result_data[result_size];
        float result_smt[result_size];
};

class SunSensor : public ModbusMaster232{
    public:
        SunSensor(uint8_t u8MBSlave, Stream &_serial) : ModbusMaster232(u8MBSlave, _serial){}
        uint32_t * get_illuminance();
};

class AirhumSensor : public ModbusMaster232{
    public:
        AirhumSensor(uint8_t u8MBSlave, Stream &_serial) : ModbusMaster232(u8MBSlave, _serial){}
        float * get_Airhumidty();
        static const int result_size = 4;
        float result_data[result_size];
        float result_smt[result_size];
};

class NPKSensor : public ModbusMaster232{
    public:
        NPKSensor(uint8_t u8MBSlave, Stream &_serial) : ModbusMaster232(u8MBSlave, _serial){}
        float * get_NPKSensor();
};

class NodeClient {
    public:
        NodeClient(String _serverURI, String _key);
        String serverURI;
        String key;
        String a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16;
        String c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16;
        String durasisleep, password;
        String endPoint;
        void sendData(float soilRes[]);
        float *packSmcData(float soil1[],float soil2[], float soil3[], float airHum1[], float lux);
        String readFile(fs::FS &fs, const char * path);
        String setEndPoint();
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
        String setA_Sens10();
        String setA_Sens11();
        String setA_Sens12();
        String setA_Sens13();
        String setC_Sens1();
        String setC_Sens2();
        String setC_Sens3();
        String setC_Sens4();
        String setC_Sens5();
        String setC_Sens6();
        String setC_Sens7();
        String setC_Sens8();
        String setC_Sens9();
        String setC_Sens10();
        String setC_Sens11();
        String setC_Sens12();
        String setC_Sens13();
        String durasi_sleep();
        String pass_word();
};

#endif