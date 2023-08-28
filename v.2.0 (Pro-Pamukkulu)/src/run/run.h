#ifndef run.h
#define run.h

#include "FS.h"
#include <SD.h>
#include <SPI.h>

#include <Arduino.h>


class sd_card {
    public:
        sd_card(int _serverURI, int _key);
        int serverURI;
        int key;
        
        void initSDCard();
        void appendFile(fs::FS &fs, const char * path, const char * message);
        void writeFile(fs::FS &fs, const char *path, const char *message);
        void cekfiledataraw();
        // void logSDCard();
        // void logSDCard2();
        // void logSDCard3();
        void cekfile3();
        void ceksleepdanpassword();
        void cekfile2();
        void cekfile();
               
};


#endif