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
    this->readHoldingRegisters(0x9, 1);
    result = this->getResponseBuffer(0)/100 - 40;
    this->clearResponseBuffer();

    return result;
}

float Aws::get_humidity()
{   
    float result = 0;
    this->readHoldingRegisters(0xA, 1);
    result = this->getResponseBuffer(0)/100;
    this->clearResponseBuffer();

    return result;
}

float Aws::get_atm_pressure()
{   
    float result = 0;
    this->readHoldingRegisters(0xB, 1);
    result = this->getResponseBuffer(0)/10;
    this->clearResponseBuffer();

    return result;
}

float Aws::get_wind_speed()
{   
    float result = 0;
    this->readHoldingRegisters(0xC, 1);
    result = this->getResponseBuffer(0)/100;
    this->clearResponseBuffer();

    return result;
}

float Aws::get_wind_dir()
{   
    float result = 0;
    this->readHoldingRegisters(0xD, 1);
    result = this->getResponseBuffer(0)/10;
    this->clearResponseBuffer();

    return result;
}

float Aws::get_rain()
{   
    float result = 0;
    this->readHoldingRegisters(0xE, 1);
    result = this->getResponseBuffer(0)/10;
    this->clearResponseBuffer();

    return result;
}

float Aws::get_illuminance()
{   
    float result = 0;
    this->readHoldingRegisters(0xF, 1);
    result = this->getResponseBuffer(0)/100;
    this->clearResponseBuffer();

    return result;
}

float * Aws::get_all(){
    const int size = 9;
    const int size2 = 4;
    static float result[size];
    static float result2[size2];
    static float buffer[size];
    buffer[2]=0.00;buffer[3]=0.00;buffer[4]=0.00;buffer[5]=0.00;buffer[8]=0.00;
    int res = 0;

    for(int i=0;i<9;i++){
        res = this->readHoldingRegisters(0x9, 8);
    
        result[0] = size;
        if (res!=0) result[1] = 0;
        else result[1] = 1;
        
        result[2] = this->getResponseBuffer(0)/100.00 - 40.00;    // temp in C
        result[3] = this->getResponseBuffer(1)/100.00;         //humidity in percent
        result[4] = this->getResponseBuffer(2)/10.00;          //atmospheric presside in hPa
        result[5] = this->getResponseBuffer(3) * 0.036;         //wind speed in km/h
        // result[5] = this->getResponseBuffer(3)/100;         //wind speed in m/s
        // result[8] = this->getResponseBuffer(6); //radiation in W/m2
        result[8] = this->getResponseBuffer(7)*0.079;         //iluminance in lux
        this->clearResponseBuffer();
        buffer[2] += result[2];
        buffer[3] += result[3];
        buffer[4] += result[4];
        buffer[5] += result[5];
        buffer[8] += result[8];
        delay(100);
        
    }
    
    res = this->readHoldingRegisters(0xD, 2);
    result2[0] = size2;
    if (res!=0) result2[1] = 0;
    else result2[1] = 1;
    
    result2[2] = this->getResponseBuffer(0)/10.00;          //wind direction in degree
    result2[3] = this->getResponseBuffer(1)*2.00/10.00;          //rainfall in mm/h from mm/5min
    this->clearResponseBuffer();

    result[6] = result2[2];
    result[7] = result2[3];

    result[2] = buffer[2]/9.00;
    result[3] = buffer[3]/9.00;
    result[4] = buffer[4]/9.00;
    result[5] = buffer[5]/9.00;
    result[8] = buffer[8]/9.00;

    return result;
}

void Aws::print_all(float data[]){
    Serial.print("status     : ");Serial.println(data[1]);
    Serial.print("temp       : ");Serial.println(data[2]);
    Serial.print("humidity   : ");Serial.println(data[3]);
    Serial.print("atm pres   : ");Serial.println(data[4]);
    Serial.print("wind speed : ");Serial.println(data[5]);
    Serial.print("wind dir   : ");Serial.println(data[6]);
    Serial.print("rainfall   : ");Serial.println(data[7]);
    Serial.print("illuminance: ");Serial.println(data[8]);
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

