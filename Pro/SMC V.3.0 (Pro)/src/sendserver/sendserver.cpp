#include "sendserver.h"
#include "datamanagement/datamanagement.h"

WiFiClient wifi;
HTTPClient http;
datamanagement data2(7);
File myfile2;

String payload, buffer, buffer3;
int delay_offline = 30000;
int nilaii1, previous_time2, cekfileada;
sendserver::sendserver(const char *server,const char *server3, String keyyy, String key)
{
    this->server_smartagri = server;
    this->server_agrieye = server3;
    this->key_smartagri = keyyy;
    this->key_agrieye = key;
}

void sendserver::connect_quicksend(const char *SSID, const char *PWD)
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, PWD);
  Serial.println("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
    {
      // Check to see if connecting failed.
      // This is due to incorrect credentials
      if (WiFi.status() == WL_CONNECT_FAILED)
      {
          Serial.println("Failed to connect to WIFI. Please verify credentials: ");
          Serial.println();
          Serial.print("SSID: ");
          Serial.println(SSID);
          Serial.print("Password: ");
          Serial.println(PWD);
          Serial.println();
      }
      delay(1000);
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Hello World, I'm connected to the internets!!");
}

void sendserver::trySending(float data[], String tanggal2, String waktu)
{
  if (WiFi.status() == WL_CONNECTED) 
  {
    Serial.println(" OK");
    String berhasilGPRS = "Berhasil tersambung dengan WIFI\n";
    data2.appendFile(SD, "/log.txt", berhasilGPRS.c_str());
    if (!wifi.connect("www.google.com", 80)) {
      Serial.println("No Internet access available");
    } else {
      Serial.println("internet access available");
      Serial.println("Mengirim ke cloud...");
      delay(500);
      kirim(data);
      previous_time2 = millis();
      int counter2 = 1;
      int counter = 1;
      myfile2 = SD.open("/datasensor.txt");
      while(myfile2.available()){

        if ((millis() - previous_time2) < delay_offline) {
            int waktudelay = delay_offline - (millis() - previous_time2);
            delay(waktudelay);
        }

        cekfileada = 1;
          
        buffer = myfile2.readStringUntil('\n');
        Serial.println(buffer);
        
        if ((counter2 == 1) || (counter2 == 2)){
          Serial.print("Mengirim ");
          Serial.println(buffer);
          kirim_datalost(buffer);
          previous_time2 = millis();
          buffer="";
          
        } else {
          buffer3 = buffer3 + buffer + '\n';
        }
        
        counter2++;
        counter++;
      }
      Serial.println("1");
      myfile2.close();
      SD.remove("/datasensor.txt");
      if((buffer3=="") && (buffer2!="")) {
        data2.writeFile(SD, "/datasensor.txt", buffer2.c_str());
      } 
      else if ((buffer2=="") && (buffer3!="")) {
        data2.writeFile(SD, "/datasensor.txt", buffer3.c_str());
      }
      else if ((buffer3 != "") && (buffer2 != "")){
        data2.writeFile(SD, "/datasensor.txt", buffer3.c_str());
        data2.appendFile(SD, "/datasensor.txt", buffer2.c_str());
      }

      String hapus = "File telah dihapus\n";
      data2.appendFile(SD, "/log.txt", hapus.c_str());
    }
  } else if (WiFi.status() != WL_CONNECTED) {
    Serial.println("tidak berhasil connect kembali");
    String tidakberhasilConnect = "tidak berhasil kembali connect ke Wifi\n";
    data2.appendFile(SD, "/log.txt", tidakberhasilConnect.c_str());
    data2.saveDataLost(tanggal2, waktu, data);
  }
}

void sendserver::quickSend(float data[])
{
 String quick = "Quicksend\n";
 data2.appendFile(SD,"/log.txt",quick.c_str());

 String req = String(this->server_smartagri) + "/?key=" + this->key_smartagri + "&C0=" + String(data[0]) + "&C1=" + 
              String(data[1]) + "&C2=" + String(data[2]) + "&C3=" + String(data[3]) 
              + "&C4=0.00&C5=" + String(data[4]) + "&C6=" + String(data[5]) + "&C7=" 
              + String(data[6]) + "&C8=" + String(data[7]) + "&C9=" + String(data[8]) 
              + "&C10=" + String(data[9]) + "&C11=" + String(data[10]);
  Serial.println(req);
  Serial.println("Performing HTTP POST request...");

  // Prepare the data to send in JSON format
  bool httpInitResult = http.begin(wifi,req);
  if( httpInitResult == false ) {
      Serial.println("http.begin() failed!"); //debug
  }

  int httpResponseCode = http.GET();
      
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
  }
  else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  data2.appendFile(SD, "/log.txt", payload.c_str()); 

  String version = getFieldFromJson(payload, "valid");
  Serial.println(version);

  if(version == "0" || version == "null"){
    Serial.println("Gagal mengirim");
  } else if(version == "1") {
    Serial.println("Berhasil mengirim");
    String tes = "0";
    data2.writeFile(SD,"/quicksend.txt",tes.c_str());
    delay(3000);
    ESP.restart();
  }
}

void sendserver::kirim(float data[]){
    this->result = data;
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("tidak berhasil connect kembali");
        String tidakberhasilConnect = "tidak berhasil kembali connect ke Wifi\n";
        data2.appendFile(SD, "/log.txt", tidakberhasilConnect.c_str());
    } else if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" OK");
        String berhasilGPRS = "Berhasil tersambung dengan WIFI\n";
        data2.appendFile(SD, "/log.txt", berhasilGPRS.c_str());
        if (!wifi.connect("www.google.com", 80)) {
          Serial.println("No Internet access available");
        } else {
          Serial.println("internet access available");
          this->state = kirim1(this->result);
          if (this->state == 1) {
            this->state2 = kirim2(this->result);
          } 
          if (this->state2 == 2) {
            kirim3(data);
          }
        }
    }
  
}

int sendserver::kirim1(float data[]){
    String req = String(this->server_smartagri) + "/?key=" + this->key_smartagri + "&C0=" + String(data[0]) + "&C1=" + 
                String(data[1]) + "&C2=" + String(data[2]) + "&C3=" + String(data[3]) 
                + "&C4=0.00&C5=" + String(data[4]) + "&C6=" + String(data[5]) + "&C7=" 
                + String(data[6]) + "&C8=" + String(data[7]) + "&C9=" + String(data[8]) 
                + "&C10=" + String(data[9]) + "&C11=" + String(data[10]);
    Serial.println(req);
    Serial.println("Performing HTTP POST request...");

    // Prepare the data to send in JSON format
    bool httpInitResult = http.begin(wifi,req);
    if( httpInitResult == false ) {
        Serial.println("http.begin() failed!"); //debug
    }

    int httpResponseCode = http.GET();
        
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
        Serial.println(payload);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    data2.appendFile(SD, "/log.txt", payload.c_str());

    String version = getFieldFromJson(payload, "valid");
    Serial.println(version);
    
    if(version == "0" || version == "null"){
        Serial.println("Gagal mengirim");
        String gagalmeng = "Gagal mengirim ke server\n";
        data2.appendFile(SD, "/log.txt", gagalmeng.c_str());
        this->statekirim = 1;
    } else if(version == "1") {
        this->statekirim = 0;
        Serial.println("Berhasil terkirim");
        String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
        data2.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
    }

    return this->statekirim;

}

int sendserver::kirim2(float data[]){
    String req = String(this->server_smartagri) + "/?key=" + this->key_smartagri + "&C0=" + String(data[0]) + "&C1=" + 
                String(data[1]) + "&C2=" + String(data[2]) + "&C3=" + String(data[3]) 
                + "&C4=0.00&C5=" + String(data[4]) + "&C6=" + String(data[5]) + "&C7=" 
                + String(data[6]) + "&C8=" + String(data[7]) + "&C9=" + String(data[8]) 
                + "&C10=" + String(data[9]) + "&C11=" + String(data[10]);
    Serial.println(req);
    Serial.println("Performing HTTP POST request...");

    // Prepare the data to send in JSON format
    bool httpInitResult = http.begin(wifi,req);
    if( httpInitResult == false ) {
        Serial.println("http.begin() failed!"); //debug
    }

    int httpResponseCode = http.GET();
        
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
        Serial.println(payload);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    data2.appendFile(SD, "/log.txt", payload.c_str());

    String version = getFieldFromJson(payload, "valid");
    Serial.println(version);
    
    if(version == "0" || version == "null"){
        Serial.println("Gagal mengirim");
        String gagalmeng = "Gagal mengirim ke server\n";
        data2.appendFile(SD, "/log.txt", gagalmeng.c_str());
        this->statekirim = 2;
    } else if(version == "1") {
        this->statekirim = 0;
        Serial.println("Berhasil terkirim");
        String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
        data2.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
    }

    return this->statekirim;

}

void sendserver::kirim3(float data[]){
    String req = String(this->server_smartagri) + "/?key=" + this->key_smartagri + "&C0=" + String(data[0]) + "&C1=" + 
                String(data[1]) + "&C2=" + String(data[2]) + "&C3=" + String(data[3]) 
                + "&C4=0.00&C5=" + String(data[4]) + "&C6=" + String(data[5]) + "&C7=" 
                + String(data[6]) + "&C8=" + String(data[7]) + "&C9=" + String(data[8]) 
                + "&C10=" + String(data[9]) + "&C11=" + String(data[10]);
    Serial.println(req);
    Serial.println("Performing HTTP POST request...");

    // Prepare the data to send in JSON format
    bool httpInitResult = http.begin(wifi,req);
    if( httpInitResult == false ) {
        Serial.println("http.begin() failed!"); //debug
    }

    int httpResponseCode = http.GET();
        
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
        Serial.println(payload);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    data2.appendFile(SD, "/log.txt", payload.c_str());

    String version = getFieldFromJson(payload, "valid");
    Serial.println(version);
    
    if(version == "0" || version == "null"){
        Serial.println("Gagal mengirim");
        String gagalmeng = "Gagal mengirim ke server\n";
        data2.appendFile(SD, "/log.txt", gagalmeng.c_str());
        this->statekirim = 3;
    } else if(version == "1") {
        this->statekirim = 0;
        Serial.println("Berhasil terkirim");
        String berhasilTerkirim = "\nBerhasil Terkirim ke Server\n";
        data2.appendFile(SD, "/log.txt", berhasilTerkirim.c_str());
    }
}

String sendserver::kirim_datalost(String buffer)
{
  Serial.println("Performing HTTP POST request...");
  Serial.println(buffer);

  String req = String(this->server_agrieye) + "/?K=" + this->key_agrieye + buffer;
  Serial.println(req);
  Serial.println("Performing HTTP POST request...");

  // Prepare the data to send in JSON format
  bool httpInitResult = http.begin(wifi,req);
  if( httpInitResult == false ) {
      Serial.println("http.begin() failed!"); //debug
  }

  int httpResponseCode = http.GET();
    
  if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      payload = http.getString();
      Serial.println(payload);
  }
  else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  data2.appendFile(SD, "/log.txt", payload.c_str());

  if(payload.indexOf("##1") == -1) {
    nilaii1 = 1;
  } else {
    nilaii1 = 2;
  }

  if (nilaii1 == 1) {
    buffer2 = buffer2 + buffer + '\n';
    Serial.println("Data tidak terkirim");
    // gagal++;
  }
  else if (nilaii1 == 2) {
    Serial.println("Data dari file terkirim nih");
  }

  return this->buffer2;
}

String sendserver::getFieldFromJson(String payload, const char* fieldName) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);
    JsonObject obj = doc.as<JsonObject>();
    
    if (obj.containsKey(fieldName)) {
        return obj[fieldName].as<String>();
    } else {
        return "null";  // Field not found, return an empty string or handle the error
    }
}

t_httpUpdate_return updateFirmware(String url_update)
{
  t_httpUpdate_return ret;

  if(WiFi.status() == WL_CONNECTED)
  {
    ret = ESPhttpUpdate.update(url_update);
    if (ret==HTTP_UPDATE_FAILED){
        Serial.println("[update] update failed.");
    } else if (ret == HTTP_UPDATE_NO_UPDATES) {
        Serial.println("[update] Update no Update");
    } else if (ret == HTTP_UPDATE_OK) {
        Serial.println("[update] Update ok.");
    }
  }

    return ret;

//     switch (ret)
//     {
//       case HTTP_UPDATE_FAILED:
//         Serial.println("[update] update failed.");
//         return ret;
//         break;

//       case HTTP_UPDATE_NO_UPDATES:
//         Serial.println("[update] Update no Update");
//         return ret;
//         break;

//       case HTTP_UPDATE_OK:
//         Serial.println("[update] Update ok.");
//         return ret;
//         break;
//     }
//   }
//   return ret;
}

void sendserver::cekUpdate(String url, String type, double firmware_version)
{
  String response;
  http.begin(url);
  http.GET();
  response = http.getString();
  http.end();
  Serial.println(response);

  String version = getFieldFromJson(response, "version");
  String url_update = getFieldFromJson(response, "url");
  Serial.println(version);
  Serial.println(url_update);

  if(version.toDouble()>firmware_version) {
    Serial.println("Update Available");
    if(updateFirmware(url_update) == HTTP_UPDATE_OK)
    {
      Serial.println("Update Success");
      ESP.restart();
    } else {
      Serial.println("Update Failed");
    }

  } else {
    Serial.println("Update not Available");
  }

}