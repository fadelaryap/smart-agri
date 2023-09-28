#include "akuisisidata.h"

ModbusMaster232 air1(1, Serial2); // Temperature, Humidity
ModbusMaster232 smc1(2, Serial2); // smc1
ModbusMaster232 smc2(3, Serial2); // smc2
ModbusMaster232 lux(4, Serial2); // luminosity
ModbusMaster232 rain(5, Serial2); // Rainfall

Adafruit_INA219 ina219;

akuisisi::akuisisi(int _SSRPin){
    this->SSRPin = _SSRPin;
}

float akuisisi::get_temp()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        air1.readHoldingRegisters(0x1, 1);
        result = air1.getResponseBuffer(0)/10.00; //Satuan Celcius
        air1.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_humidity()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        air1.readHoldingRegisters(0x0, 1);
        result = air1.getResponseBuffer(0)/10.00;  //Satuan %
        air1.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_soil_temp_1()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        smc1.readHoldingRegisters(0x0, 1);
        result = smc1.getResponseBuffer(0)/100.00;  //Satuan Celcius
        smc1.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_soil_moist_1()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        smc1.readHoldingRegisters(0x1, 1);
        result = smc1.getResponseBuffer(0)/100.00;  //Satuan %
        smc1.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_soil_temp_2()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        smc2.readHoldingRegisters(0x0, 1);
        result = smc2.getResponseBuffer(0)/100.00;  //Satuan Celcius
        smc2.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_soil_moist_2()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        smc2.readHoldingRegisters(0x1, 1);
        result = smc2.getResponseBuffer(0)/100.00;  //Satuan %
        smc2.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_rain()
{
    float result = 0.00;
    aws3.readHoldingRegisters(0x1, 1);
    result = aws3.getResponseBuffer(0)/50.00;  //Satuan mm/5 menit
    aws3.clearResponseBuffer();

    aws3.writeSingleRegister(0x6,1);
    
    return result;
}

float akuisisi::get_lux()
{
    float result = 0.00;
    float result2 = 0.00;
    uint16_t high=0;
    uint16_t low=0;
    uint32_t illu = 0;
    for(int a=0;a<9;a++)
    {
        lux.readHoldingRegisters(0x0, 2);
        high = lux.getResponseBuffer(0);  //Satuan lux
        lux.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_pm25()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        aws5.readHoldingRegisters(0x1, 1);
        result = aws5.getResponseBuffer(0)/1.00;  //PM 2.5
        aws5.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_pm10()
{
    float result = 0.00;
    float result2 = 0.00;
    for(int a=0;a<9;a++)
    {
        aws5.readHoldingRegisters(0x2, 1);
        result = aws5.getResponseBuffer(0)/1.00;  //PM 10
        aws5.clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float akuisisi::get_voltage(){
    float result = ina219.getBusVoltage_V();
    return result;
}

float akuisisi::get_power(){
    float result = ina219.getPower_mW();
    return result;
}

float * akuisisi::get_all()
{
    static float result[11];
    result[0] = get_voltage();
    result[1] = get_radiance();
    result[2] = get_atm_pressure();
    result[3] = get_power();
    result[4] = get_temp();
    result[5] = get_humidity();
    result[6] = get_rain();
    result[7] = get_wind_speed();
    result[8] = get_wind_dir();
    result[9] = get_pm25();
    result[10] = get_pm10();

    return result;
}

void akuisisi::print_all(float data[])
{
    Serial.print("temp       : ");Serial.println(data[4]);
    Serial.print("humidity   : ");Serial.println(data[5]);
    Serial.print("atm pres   : ");Serial.println(data[2]);
    Serial.print("wind speed : ");Serial.println(data[7]);
    Serial.print("wind dir   : ");Serial.println(data[8]);
    Serial.print("rainfall   : ");Serial.println(data[6]);
    Serial.print("radiance   : ");Serial.println(data[1]);
    Serial.print("pm 2.5     : ");Serial.println(data[9]);
    Serial.print("pm 10      : ");Serial.println(data[10]);
    Serial.print("voltage    : ");Serial.println(data[0]);
    Serial.print("power      : ");Serial.println(data[3]);
    Serial.println("+++++++++++++++++++++");
}

String akuisisi::print(float data[])
{
  String nilaisave = "Suhu = " + String(data[4]) + ", Humidit = " + String(data[5]) + ", Atm Pressure = " + String(data[2]) + ", Wind Speed = " + String(data[7]) + ", Wind Direction = " + String(data[8]) + ", Rainfall = " + String(data[6]) + ", Radiance = " + String(data[1]) + ", pm 2.5 = " + String(data[9]) + ", pm 10 = " + String(data[10]) + ", Voltage = " + String(data[0]) + ", Power = " + String(data[3]) + "\n";
  return nilaisave;
}

void akuisisi::setupSensor(){
    Serial2.begin(9600);
    pinMode(this->SSRPin, OUTPUT);
}

void akuisisi::setupBegin(){
    aws1.begin();
    aws2.begin();
    aws3.begin();
    aws4.begin();
    aws5.begin();
    ina219.begin();
}

void akuisisi::turnOnSensor(){
    digitalWrite(this->SSRPin, HIGH);
}

void akuisisi::turnOffSensor(){
    digitalWrite(this->SSRPin, LOW);
}


