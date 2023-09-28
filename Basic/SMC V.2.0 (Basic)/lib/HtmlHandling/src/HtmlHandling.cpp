#include "HtmlHandling.h"

String HtmlHandling::processor(const String &var)
{
    if (var == "endpoint")
    {
        return readFile(SD, "/endpoint.txt");
    }
    else if (var == "api_key")
    {
        return readFile(SD, "/api_key.txt");
    }
    if (var == "durasisleep")
    {
        return readFile(SD, "/durasi_sleep.txt");
    }
    else if (var == "password")
    {
        return readFile(SD, "/pass_word.txt");
    }
    return String();
}

String HtmlHandling::download()
{
  SD.remove("/log.txt");
  return String();
}

String HtmlHandling::readSens1(){
  return readFile(SD, "/SENS1.txt");
}

String HtmlHandling::readSens2(){
  return readFile(SD, "/SENS2.txt");
}

String HtmlHandling::readSens3(){
  return readFile(SD, "/SENS3.txt");
}

String HtmlHandling::readSens4(){
  return readFile(SD, "/SENS4.txt");
}

String HtmlHandling::readSens5(){
  return readFile(SD, "/SENS5.txt");
}

String HtmlHandling::readSens6(){
  return readFile(SD, "/SENS6.txt");
}

String HtmlHandling::readSens7(){
  return readFile(SD, "/SENS7.txt");
}

String HtmlHandling::readSens8(){
  return readFile(SD, "/SENS8.txt");
}

String HtmlHandling::readSens9(){
  return readFile(SD, "/SENS9.txt");
}

String HtmlHandling::readSens10(){
  return readFile(SD, "/SENS10.txt");
}

String HtmlHandling::readSens11(){
  return readFile(SD, "/SENS11.txt");
}

String HtmlHandling::readSens12(){
  return readFile(SD, "/SENS12.txt");
}

String HtmlHandling::readSens13(){
  return readFile(SD, "/SENS13.txt");
}

String HtmlHandling::readSens14(){
  return readFile(SD, "/SENS14.txt");
}

String HtmlHandling::readSens15(){
  return readFile(SD, "/SENS15.txt");
}

String HtmlHandling::readSens16(){
  return readFile(SD, "/SENS16.txt");
}

String HtmlHandling::readVolt(){
  return readFile(SD, "/VOLTAGE.txt");
}

String HtmlHandling::readDAYA(){
  return readFile(SD, "/DAYA.txt");
}

String HtmlHandling::readRSSI(){
  return readFile(SD, "/RSSI.txt");
}

String HtmlHandling::readFile(fs::FS &fs, const char *path)
{
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

void HtmlHandling::cek(){
    int a = 0;
}

void HtmlHandling::writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);
    File file = fs.open(path, "w");
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
}

void HtmlHandling::hapusFile(){
  String nilaii = "";
  SD.remove("/log.txt");
  writeFile(SD,"/log.txt", nilaii.c_str());
}