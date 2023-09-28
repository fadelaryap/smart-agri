#include "smc.h"
#include "modbus/ModbusMaster232.h"
#include <HTTPClient.h>
#include "FS.h"
#include <SD.h>
#include <SPI.h>


void SoilSensor::setupRelay(int _relayPin){
    this->relayPin = _relayPin;
    pinMode(this->relayPin, OUTPUT);
}


void SoilSensor::turnOnOFF(bool state){
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

void SoilSensor::diedSensor(){
    this->turnOnOFF(false);
    
}

void SoilSensor::hidupSensor(){
    this->turnOnOFF(true);
    
}

float SoilSensor::get_temp()
{   
    float temperature = 0;
    this->readHoldingRegisters(0x0, 1);
    temperature = this->getResponseBuffer(0)/100;
    this->clearResponseBuffer();

    return temperature;
}

float SoilSensor::get_moist(){
    float moist = 0;
    this->readHoldingRegisters(0x1, 2);
    moist = this->getResponseBuffer(0)/100;
    this->clearResponseBuffer();

    return moist;
}

float * SoilSensor::get_all(){    
    int res = 0;
    this->result_smt[2] = 0.00;this->result_smt[3] = 0.00;
    for(int i=0;i<9;i++) {
        res = this->readHoldingRegisters(0x0, 2);

        this->result_data[0] = this->result_size;

        if (res!=0) this->result_data[1] = 0;
        else this->result_data[1] = 1;

        this->result_data[2] = this->getResponseBuffer(0)/100.00; // temperature in C
        this->result_data[3] = this->getResponseBuffer(1)/100.00; // moisture in percent
        this->clearResponseBuffer();
        this->result_smt[2] += this->result_data[2];
        this->result_smt[3] += this->result_data[3]; 
        delay(200);
    }
    this->result_data[2] = this->result_smt[2]/9.00;
    this->result_data[3] = this->result_smt[3]/9.00;

    return this->result_data;
}   

float * NPKSensor::get_NPKSensor(){
    const int size = 9;
    static float result[size];
    static float nilai[size];
    int res = 0;
    // float nilai;
    nilai[0] = size;
    nilai[1] = 1;
    nilai[2] = 0.00;nilai[3] = 0.00;nilai[4] = 0.00;nilai[5] = 0.00;nilai[6]= 0.00;nilai[7] = 0.00;nilai[8] = 0.00;

    for(int i=0;i<9;i++){
        res = this->readHoldingRegisters(0x2, 7);
        result[0] = size;
        if (res!= 0) result[1] = 0;
        else result[1] = 1;
        result[6] = this->getResponseBuffer(0);
        result[2] = this->getResponseBuffer(1)/10.00; //PH
        result[3] = this->getResponseBuffer(2); //Nitrogen in mg/Kg
        result[4] = this->getResponseBuffer(3); //Phosphorus in mg/Kg
        result[5] = this->getResponseBuffer(4); //Potasium/Kalium in mg/Kg
        result[7] = this->getResponseBuffer(5);
        result[8] = this->getResponseBuffer(6);
        this->clearResponseBuffer();
        nilai[2] += result[2];
        nilai[3] += result[3];
        nilai[4] += result[4];
        nilai[5] += result[5];
        nilai[6] += result[6];
        nilai[7] += result[7];
        nilai[8] += result[8];

        delay(200);
    }

    result[2] = nilai[2]/9.00;
    result[3] = nilai[3]/9.00;
    result[4] = nilai[4]/9.00;
    result[5] = nilai[5]/9.00;
    result[6] = nilai[6]/9.00;
    result[7] = nilai[7]/9.00;
    result[8] = nilai[8]/9.00;

    return result;
}


uint32_t * SunSensor::get_illuminance(){
    static uint32_t result[3];
    
    uint32_t illu = 0;
    uint16_t illu_buf = 0;
    int res = 0;
    // float illu = 0;

    res = this->readHoldingRegisters(0x0, 2);
    if (res!=0) result[1] = 0;
    else result[1] = 1;


    illu_buf = this->getResponseBuffer(0);
    // Serial.print('\t');
    // Serial.print("High: "); Serial.print(illu_buf, HEX); Serial.print('\t');
    illu = (illu | illu_buf) << 16;

    illu_buf = this->getResponseBuffer(1);
    // Serial.print("Low: "); Serial.print(illu_buf, HEX); Serial.print('\t'); Serial.print("illu: ");
    illu = (illu | illu_buf);

    result[2] = illu;
    this->clearResponseBuffer();

    return result;
}

float * AirhumSensor::get_Airhumidty(){    
    int res = 0;
    this->result_smt[2] = 0.00;this->result_smt[3] = 0.00;
    for(int i=0;i<9;i++) {
        res = this->readHoldingRegisters(0x0, 2);

        this->result_data[0] = this->result_size;

        if (res!=0) this->result_data[1] = 0;
        else this->result_data[1] = 1;

        this->result_data[2] = this->getResponseBuffer(0)/10.00; // moisture in percent
        this->result_data[3] = this->getResponseBuffer(1)/10.00; // temperature in C 
        this->clearResponseBuffer();
        this->result_smt[2] += this->result_data[2];
        this->result_smt[3] += this->result_data[3];
        delay(200);
    }
    this->result_data[2] = this->result_smt[2]/9.00;
    this->result_data[3] = this->result_smt[3]/9.00;

    return this->result_data;
}  

NodeClient::NodeClient(String _serverURI, String _key){
    this->serverURI = _serverURI;
    this->key = _key;
}

void NodeClient::sendData(float soilRes[]){
    int soilRes_size = int(soilRes[0]);
    int allData_size = soilRes_size;
    
    String data_req = "";

    for (int i=2; i<allData_size; i++){
        char buf[50];
        if (i < soilRes_size){
            sprintf(buf, "&C%d=%.2f",i-2,soilRes[i]);
        } 
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

float * NodeClient::packSmcData(float soil1[], float soil2[], float soil3[], float airHum1[], float lux){
    const int size = 8;
    static float result[size];

    result[0] = size;

    if ((soil1[1] == 1)&& (soil2[1] == 1) && (soil3[1] == 1)) result[1] = 1;
    else result[1] = 0;

    if(airHum1[3] < 0.00){airHum1[3] = 0.00;}
    if(airHum1[2] < 0.00){airHum1[2] = 0.00;}
    if(soil1[3] < 0.00){soil1[3] = 0.00;}
    if(soil2[3] < 0.00){soil2[3] = 0.00;}
    if(soil3[3] < 0.00){soil3[3] = 0.00;}
    if(lux < 0.00){lux = 0.00;}

    result[2] = airHum1[3]; //temperature
    result[3] = airHum1[2]; //air moist
    result[4] = lux;
    result[5] = soil1[3]; //only moist
    result[6] = soil2[3];//only moist
    result[7] = soil3[3];//only moist
    

    // result[4] =

    return result;
}

String NodeClient::readFile(fs::FS &fs, const char *path){
    Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path, "r");
    if (!file || file.isDirectory())
    {
        Serial.println("- empty file or failed to open file");
        return String();
    }
    Serial.println("- read from file:");
    String fileContent;
    while (file.available())
    {
        fileContent += String((char)file.read());
    }
    Serial.println(fileContent);
    return fileContent;
}

String NodeClient::setEndPoint(){
    this->endPoint = this->readFile(SD, "/endpoint.txt");
    return this->endPoint;
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

String NodeClient::setA_Sens10(){

    this->a10 = this->readFile(SD, "/a_10.txt");
    return this->a10;
}

String NodeClient::setC_Sens10(){

    this->c10 = this->readFile(SD, "/c_10.txt");
    return this->c10;
}

String NodeClient::setA_Sens11(){

    this->a11 = this->readFile(SD, "/a_11.txt");
    return this->a11;
}

String NodeClient::setC_Sens11(){

    this->c11 = this->readFile(SD, "/c_11.txt");
    return this->c11;
}

String NodeClient::setA_Sens12(){

    this->a12 = this->readFile(SD, "/a_12.txt");
    return this->a12;
}

String NodeClient::setC_Sens12(){

    this->c12 = this->readFile(SD, "/c_12.txt");
    return this->c12;
}

String NodeClient::setA_Sens13(){

    this->a13 = this->readFile(SD, "/a_13.txt");
    return this->a13;
}

String NodeClient::setC_Sens13(){

    this->c13 = this->readFile(SD, "/c_13.txt");
    return this->c13;
}

String NodeClient::durasi_sleep(){

    this->durasisleep = this->readFile(SD, "/durasi_sleep.txt");
    return this->durasisleep;
}

String NodeClient::pass_word(){

    this->password = this->readFile(SD, "/pass_word.txt");
    return this->password;
}

