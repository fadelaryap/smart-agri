#ifndef SENDSERVER_H
#define SENDSERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>

class sendserver{
    public:
        sendserver(const char *server, const char *server3, String keyyy, String key);
        void trySending(float data[], String tanggal2, String waktu);
        void quickSend(float data[]);
        void kirim(float data[]);
        int kirim1(float data[]);
        int kirim2(float data[]);
        void kirim3(float data[]);
        String kirim_datalost(String buffer);
        void cekUpdate(String url, String type, double firmware_version);
        String getFieldFromJson(String payload, const char* fieldName);
        void connect_quicksend(const char *SSID, const char *PWD);
        float *result;
        int statekirim, state, state2, state3;
        String buffer2, key_smartagri, key_agrieye;
        const char *server_smartagri;
        const char *server_agrieye;
};

#endif