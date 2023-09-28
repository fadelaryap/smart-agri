#ifndef READANDFILTER_H
#define READANDFILTER_H

#include <Arduino.h>
#include "modbus/ModbusMaster232.h"
#include <Adafruit_INA219.h>

class akuisisi : public ModbusMaster232{
    public:
        akuisisi(int _SSRPin);
        float get_temp();
        float get_humidity();
        float get_soil_temp_1();
        float get_soil_moist_1();
        float get_soil_temp_2();
        float get_soil_moist_2();
        float get_rain();
        float get_lux();
        float get_voltage();
        float get_power();
        float * get_all();
        void print_all(float data[]);
        String print(float data[]);
        void setupSensor();
        void setupBegin();
        void turnOnSensor();
        void turnOffSensor();
        int SSRPin;

};

#endif