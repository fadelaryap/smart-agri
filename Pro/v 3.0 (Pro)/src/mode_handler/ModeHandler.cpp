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
            this->isMaintanance = false;
            digitalWrite(this->ledPin, HIGH);
            Serial.println("Maintanance State is True!");
        }
        else
        {
            this->isMaintanance = true;
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
    } else {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid, password);
        Serial.println("Connecting...");
        int counter = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            // Check to see if connecting failed.
            // This is due to incorrect credentials
            if (WiFi.status() == WL_CONNECT_FAILED)
            {
                Serial.println("Failed to connect to WIFI. Please verify credentials: ");
                Serial.println();
                Serial.print("SSID: ");
                Serial.println(ssid);
                Serial.print("Password: ");
                Serial.println(password);
                Serial.println();
            }
            delay(1000);
            counter++;
            if(counter == 120) {
                break;
            }
        }

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        Serial.println("Hello World, I'm connected to the internets!!");
    }
    


}


