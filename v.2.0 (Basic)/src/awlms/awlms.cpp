#include "awlms.h"
#include <HTTPClient.h>
#include "FS.h"
#include <SD.h>
#include <SPI.h>

AW::AW(int ASIK, String WOW){
    this->PIN = ASIK;
    this->nilai_f = WOW;
}

void AW::setupRelay(int _relayPin){
    this->relayPin = _relayPin;
    pinMode(this->relayPin, OUTPUT);
}


void AW::turnOnOFF(bool state){
    // relay is assumed to be Active High

    if (state){
        // change HIGH to LOW if relay Active Low
        digitalWrite(this->relayPin, LOW);
    }
    else{
        //change LOW to HIGH if relay Active Low
        digitalWrite(this->relayPin, HIGH);
    }
    
}

void AW::diedSensor(){
    this->turnOnOFF(false);
    
}

void AW::hidupSensor(){
    this->turnOnOFF(true);
    
}

float AW::bacaData(){
    float sensorValue = analogRead(this->PIN);
    return sensorValue;
}

NodeClient::NodeClient(String _serverURI, String _key){
    this->serverURI = _serverURI;
    this->key = _key;
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

String NodeClient::setTinggi(){

    // this->serverURI = this->readFile(SPIFFS, "/endpoint.txt");
    this->key = this->readFile(SD, "/tinggi.txt");
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



String NodeClient::durasi_sleep(){

    this->durasisleep = this->readFile(SD, "/durasi_sleep.txt");
    return this->durasisleep;
}

String NodeClient::pass_word(){

    this->password = this->readFile(SD, "/pass_word.txt");
    return this->password;
}

String NodeClient::readID(){

    this->idd_data = this->readFile(SD, "/id_data.txt");
    return this->idd_data;
}

