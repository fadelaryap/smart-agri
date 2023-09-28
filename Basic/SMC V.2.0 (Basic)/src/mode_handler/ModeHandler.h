#ifndef Mode_Handler.h
#define Mode_Handler.h

#include <Arduino.h>
#include "WiFi.h"
// #include <TinyGsmClient.h>

class ModeHandler{
    public:
        ModeHandler(int _buttonPin, int _ledPin);
       
        int buttonPin;
        int ledPin;
        bool isMaintanance;
        
        void setupButton();
        bool getMaintananceState(bool update=false);
        void chooseWifiSetting(const char* ssid, const char* password);
        
};


#endif
