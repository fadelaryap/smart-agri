#include "aws.h"
#include "modbus/ModbusMaster232.h"
#include <HTTPClient.h>
#include "FS.h"
#include <SD.h>
#include <SPI.h>

void Aws::setupRelay(int _relayPin){
    this->relayPin = _relayPin;
    pinMode(this->relayPin, OUTPUT);
}


void Aws::turnOnOFF(bool state){
    // relay is assumed to be Active High

    if (state){
        // change HIGH to LOW if relay Active Low
        digitalWrite(this->relayPin, HIGH);
    }
    else{
        //change LOW to HIGH if relay Active Low
        digitalWrite(this->relayPin, LOW);
    }
    
}

void Aws::restartSensor(){
    this->turnOnOFF(false);
    
}

void Aws::hidupSensor(){
    this->turnOnOFF(true);
    
}


float Aws::get_temp()
{   
    float result = 0;
    float result2 =0;
    for(int a = 0; a<9; a++){
        this->readHoldingRegisters(0x0, 1);
        result = this->getResponseBuffer(0)/100.00; //Satuan Celcius
        this->clearResponseBuffer();
        result2 += result;
        // delay(100);
    }
    
    result=result2/9.00;

    return result;
}

float Aws::get_humidity()
{   
    float result = 0;
    float result2 = 0;
    for(int a=0;a<9;a++){
        this->readHoldingRegisters(0x1, 1);
        result = this->getResponseBuffer(0)/100.00; //Satuan %
        this->clearResponseBuffer();
        result2 += result;
        delay(100);
    }
    
    result = result2/9.00;

    return result;
}

float Aws::get_atm_pressure()
{   
    float result = 0;
    float result2 = 0;
    for(int a=0;a<9;a++){
        this->readHoldingRegisters(0x2, 1);
        result = this->getResponseBuffer(0)/10.00; //Satuan hPa
        this->clearResponseBuffer();
        result2 += result;
        delay(100);
    }
    result = result2/9.00;

    return result;
}

float Aws::get_wind_speed()
{   
    float result = 0;
    float result2 = 0;
    for(int a=0;a<9;a++){
        this->readHoldingRegisters(0x0, 1);
        result = this->getResponseBuffer(0)* 0.036; //Satuan Km/h
        // result = this->getResponseBuffer(0)/100.00; //Satuan m/s
        this->clearResponseBuffer();
        result2 += result;
        delay(100);
    }

    result = result2/9.00;
    
    return result;
}

float Aws::get_wind_dir()
{   
    float result = 0;
    this->readHoldingRegisters(0x1, 1);
    result = this->getResponseBuffer(0)/1.00; //Satuan derajat
    this->clearResponseBuffer();

    return result;
}

float Aws::get_rain()
{   
    float result = 0;
    this->readHoldingRegisters(0x1, 1);
    result = this->getResponseBuffer(0)/50.00;
    this->clearResponseBuffer();

    // this->writeSingleRegister(0x6,1);

    return result;
}

float Aws::get_radiance()
{   
    float result = 0;
    float result2 = 0;
    for(int a=0;a<9;a++){
        this->readHoldingRegisters(0x5, 1);
        result = this->getResponseBuffer(0)/1.00; //Satuan W/m2
        this->clearResponseBuffer();
        result2 += result;
        delay(100);
    }
    result = result2/9.00;

    return result;
}

float Aws::get_pm25()
{   
    float result = 0;
    float result2 = 0;
    for(int a=0;a<9;a++){
        this->readHoldingRegisters(0x1, 1);
        result = this->getResponseBuffer(0)/1.00; //PM 2.5
        this->clearResponseBuffer();
        result2 += result;
        delay(100);
    }
    result = result2/9.00;

    return result;
}

float Aws::get_pm10()
{   
    float result = 0;
    float result2 = 0;
    for(int a=0;a<9;a++){
        this->readHoldingRegisters(0x2, 1);
        result = this->getResponseBuffer(0)/1.00; //PM 10
        this->clearResponseBuffer();
        result2 += result;
        delay(100);
    }
    result = result2/9.00;

    return result;
}

// float Aws::get_sunduration()
// {   
//     float result = 0;
//     float result2 = 0;
//     for(int a=0;a<9;a++){
//         this->readHoldingRegisters(0x2, 1);
//         result = this->getResponseBuffer(0)/1.00; //PM 10
//         this->clearResponseBuffer();
//         result2 += result;
//         delay(100);
//     }
//     result = result2/9.00;

//     return result;
// }

float * Aws::get_all(){
    const int size = 11;
    const int size2 = 4;
    static float result[size];
    static float result2[size2];
    static float buffer[size];
    buffer[2]=0.00;buffer[3]=0.00;buffer[4]=0.00;buffer[5]=0.00;buffer[8]=0.00;
    int res = 0;

    result[0] = size;
    if (res!=0) result[1] = 0;
        else result[1] = 1;
        
        result[2] = get_temp();    // temp in C
        result[3] = get_humidity();         //humidity in percent
        result[4] = get_atm_pressure();          //atmospheric presside in hPa
        
    
    return result;
}

float * Aws::get_all2(){
    const int size = 11;
    const int size2 = 4;
    static float result[size];
    static float result2[size2];
    static float buffer[size];
    buffer[2]=0.00;buffer[3]=0.00;buffer[4]=0.00;buffer[5]=0.00;buffer[8]=0.00;
    int res = 0;

    result[0] = size;
    if (res!=0) result[1] = 0;
        else result[1] = 1;
        
        
        result[5] = get_wind_speed();         //wind speed in km/h
        
        
    
    
    result[6] = get_wind_dir();          //wind direction in degree
           //rainfall in mm/h from mm/5min
    
    return result;
}

float * Aws::get_all3(){
    const int size = 11;
    const int size2 = 4;
    static float result[size];
    static float result2[size2];
    static float buffer[size];
    buffer[2]=0.00;buffer[3]=0.00;buffer[4]=0.00;buffer[5]=0.00;buffer[8]=0.00;
    int res = 0;

    result[0] = size;
    if (res!=0) result[1] = 0;
        else result[1] = 1;
        
        
    result[7] = get_rain();          //rainfall in mm/h from mm/5min
    
    return result;
}

float * Aws::get_all4(){
    const int size = 11;
    const int size2 = 4;
    static float result[size];
    static float result2[size2];
    static float buffer[size];
    buffer[2]=0.00;buffer[3]=0.00;buffer[4]=0.00;buffer[5]=0.00;buffer[8]=0.00;
    int res = 0;

    result[0] = size;
    if (res!=0) result[1] = 0;
        else result[1] = 1;
        
       
        result[8] = get_radiance();         //iluminance in lux
        
    
    return result;
}

float * Aws::get_all5(){
    const int size = 11;
    const int size2 = 4;
    static float result[size];
    static float result2[size2];
    static float buffer[size];
    buffer[2]=0.00;buffer[3]=0.00;buffer[4]=0.00;buffer[5]=0.00;buffer[8]=0.00;
    int res = 0;

    result[0] = size;
    if (res!=0) result[1] = 0;
        else result[1] = 1;
        
    
        result[9] = get_pm25();
        result[10] = get_pm10();
      
    
    return result;
}

// float * Aws::get_all6(){
//     const int size = 12;
//     const int size2 = 4;
//     static float result[size];
//     static float result2[size2];
//     static float buffer[size];
//     buffer[2]=0.00;buffer[3]=0.00;buffer[4]=0.00;buffer[5]=0.00;buffer[8]=0.00;
//     int res = 0;

//     result[0] = size;
//     if (res!=0) result[1] = 0;
//         else result[1] = 1;
        
    
//         result[11] = get_sunduration();
        
      
    
//     return result;
// }

void Aws::print_all(float data[]){
    Serial.print("status     : ");Serial.println(data[1]);
    // static float result[11];

    // result[2] = get_temp();
    // result[3] = get_humidity();
    // result[4] = get_atm_pressure();
    // result[5] = get_wind_speed();
    // result[6] = get_wind_dir();
    // result[7] = get_rain();
    // result[8] = get_radiance();
    // result[9] = get_pm25();
    // result[10] = get_pm10();

    Serial.print("temp       : ");Serial.println(data[2]);
    Serial.print("humidity   : ");Serial.println(data[3]);
    Serial.print("atm pres   : ");Serial.println(data[4]);
    Serial.print("wind speed : ");Serial.println(data[5]);
    Serial.print("wind dir   : ");Serial.println(data[6]);
    Serial.print("rainfall   : ");Serial.println(data[7]);
    Serial.print("radiance   : ");Serial.println(data[8]);
    Serial.print("pm 2.5     : ");Serial.println(data[9]);
    Serial.print("pm 10      : ");Serial.println(data[10]);
    Serial.println("+++++++++++++++++++++");
}

NodeClient::NodeClient(String _serverURI, String _key){
    this->serverURI = _serverURI;
    this->key = _key;
}

void NodeClient::sendData(float data[]){
    String data_req = "";

    for (int i=2; i<data[0]; i++){
        char buf[50];
        sprintf(buf, "&C%d=%.2f",i-2,data[i]);
        data_req += buf;
    }

    String req = this->serverURI + "?K=" + this->key + data_req;
    Serial.println("req:");
    Serial.println(req);

    HTTPClient http;
    WiFiClient wifi;
    
    // Your Domain name with URL path or IP address with path

    bool httpInitResult = http.begin(wifi, req);
    if( httpInitResult == false ) {
        Serial.println("http.begin() failed!"); //debug
    }
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();


}


String NodeClient::readFile(fs::FS &fs, const char *path){
    // Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path, "r");
    if (!file || file.isDirectory())
    {
        Serial.println("- empty file or failed to open file");
        return String();
    }
    // Serial.println("- read from file:");
    String fileContent;
    while (file.available())
    {
        fileContent += String((char)file.read());
    }
    // Serial.println(fileContent);
    return fileContent;
}

String NodeClient::setApiFromFile(){

    // this->serverURI = this->readFile(SPIFFS, "/endpoint.txt");
    this->key = this->readFile(SD, "/api_key.txt");
    return this->key;
}

String NodeClient::setA_Sens1(){

    this->a1 = this->readFile(SD, "/a_1.txt");
    return this->a1;
}

String NodeClient::setC_Sens1(){

    this->c1 = this->readFile(SD, "/c_1.txt");
    return this->c1;
}

String NodeClient::setA_Sens2(){

    this->a2 = this->readFile(SD, "/a_2.txt");
    return this->a2;
}

String NodeClient::setC_Sens2(){

    this->c2 = this->readFile(SD, "/c_2.txt");
    return this->c2;
}

String NodeClient::setA_Sens3(){

    this->a3 = this->readFile(SD, "/a_3.txt");
    return this->a3;
}

String NodeClient::setC_Sens3(){

    this->c3 = this->readFile(SD, "/c_3.txt");
    return this->c3;
}

String NodeClient::setA_Sens4(){

    this->a4 = this->readFile(SD, "/a_4.txt");
    return this->a4;
}

String NodeClient::setC_Sens4(){

    this->c4 = this->readFile(SD, "/c_4.txt");
    return this->c4;
}

String NodeClient::setA_Sens5(){

    this->a5 = this->readFile(SD, "/a_5.txt");
    return this->a5;
}

String NodeClient::setC_Sens5(){

    this->c5 = this->readFile(SD, "/c_5.txt");
    return this->c5;
}

String NodeClient::setA_Sens6(){

    this->a6 = this->readFile(SD, "/a_6.txt");
    return this->a6;
}

String NodeClient::setC_Sens6(){

    this->c6 = this->readFile(SD, "/c_6.txt");
    return this->c6;
}

String NodeClient::setA_Sens7(){

    this->a7 = this->readFile(SD, "/a_7.txt");
    return this->a7;
}

String NodeClient::setC_Sens7(){

    this->c7 = this->readFile(SD, "/c_7.txt");
    return this->c7;
}

String NodeClient::setA_Sens8(){

    this->a8 = this->readFile(SD, "/a_8.txt");
    return this->a8;
}

String NodeClient::setC_Sens8(){

    this->c8 = this->readFile(SD, "/c_8.txt");
    return this->c8;
}

String NodeClient::setA_Sens9(){

    this->a9 = this->readFile(SD, "/a_9.txt");
    return this->a9;
}

String NodeClient::setC_Sens9(){

    this->c9 = this->readFile(SD, "/c_9.txt");
    return this->c9;
}

String NodeClient::durasi_sleep(){

    this->durasisleep = this->readFile(SD, "/durasi_sleep.txt");
    return this->durasisleep;
}

String NodeClient::pass_word(){

    this->password = this->readFile(SD, "/pass_word.txt");
    return this->password;
}

int NodeClient::mapRSSItoPercentage(int rssi, int rssiMin, int rssiMax) {
  int percentage = map(rssi, rssiMin, rssiMax, 0, 100);
  percentage = constrain(percentage, 0, 100);
  return percentage;
}

