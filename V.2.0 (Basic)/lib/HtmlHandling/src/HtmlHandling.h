
#ifndef HtmlHandling
#define ESPDash_h
#include "FS.h"
#include <SD.h>
#include <SPI.h>
#include "Arduino.h"

#if defined(ESP8266)
    #include <Hash.h>
    #include <FS.h>
#elif defined(ESP32)
    #include <SPIFFS.h>
#endif


class HtmlHandling{
    public:
        static String processor(const String& var);
        static String readFile(fs::FS &fs, const char * path);
        static String download();
        static void writeFile(fs::FS &fs, const char * path, const char * message);
        static String readSens1();
        static String readSens2();
        static String readSens3();
        static String readSens4();
        static String readSens5();
        static String readSens6();
        static String readSens7();
        static String readSens8();
        static String readSens9();
        static String readSens10();
        static String readSens11();
        static String readSens12();
        static String readSens13();
        static String readSens14();
        static String readSens15();
        static String readSens16();
        static String readVolt();
        static String readDAYA();
        static String readRSSI();
        void cek();
        void hapusFile();
};

#endif