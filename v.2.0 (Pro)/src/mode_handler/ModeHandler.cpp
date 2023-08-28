#include "ModeHandler.h"
int datatombol[10];
int aaa = 0;

ModeHandler::ModeHandler(int _buttonPin, int _ledPin)
{
    this->buttonPin = _buttonPin;
    this->ledPin = _ledPin;
    this->isMaintanance = false;
}

void ModeHandler::setupButton()
{
    pinMode(this->buttonPin, INPUT);
    pinMode(this->ledPin, OUTPUT);

}

bool ModeHandler::getMaintananceState(bool update)
{
    if (update)
    {
        int buttonState = analogRead(this->buttonPin);
        
        if (buttonState == LOW)
        {
            this->isMaintanance = true;
            digitalWrite(this->ledPin, HIGH);
            Serial.println("Maintanance State is True!");
        }
        else
        {
            this->isMaintanance = false;
            digitalWrite(this->ledPin, LOW);
            Serial.println("Maintanance State is False!");
        }
    }

    return this->isMaintanance;
}

void ModeHandler::chooseWifiSetting(const char *ssid, const char *password)
{
    if (this->isMaintanance)
    {
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
        WiFi.softAP(ssid, password);
        Serial.print("IP Address: ");
        Serial.println(WiFi.softAPIP());
    }


}
