#include "ESPDash.h"

// Integral type to string pairs events
// ID, type

// Integral type to string pairs events
// ID, type




/*
  Constructor
*/
ESPDash::ESPDash(AsyncWebServer* server, bool enable_stats) {
  _server = server;
  

  // Initialize AsyncWebSocket
  _ws = new AsyncWebSocket("/dashws");

  // #ifdef ESP32
  


  // Attach AsyncWebServer Routes
  // _server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request){
  //   if(basic_auth){
  //     if(!request->authenticate(username, password))
  //     return request->requestAuthentication();
  //   }
  //   // respond with the compressed frontend
  //   AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", DASH_HTML, DASH_HTML_SIZE);
  //   response->addHeader("Content-Encoding","gzip");
  //   request->send(response);        
  // });

  _server->on("/set_api", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    Serial.print("Content type::");
    Serial.println(request->contentType());
    Serial.println("OFF hit.");

    int params = request->params();
    Serial.printf("%d params sent in\n", params);
      
    String endpoint;
    String apiKey;
    String PARAM_INPUT_1 = "endpoint";
    String PARAM_INPUT_2 = "apiKey";
    // // GET input1 value on <ESP_IP>/update?output=<endpoint>&state=<apiKey>

    if (request->hasParam(PARAM_INPUT_1)){
      endpoint = request->getParam(PARAM_INPUT_1)->value();
      Serial.printf("endpoint: %s/n", endpoint);
      if(endpoint != ""){
      this->htmlHandler->writeFile(SD, "/endpoint.txt", endpoint.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_2)){
      apiKey = request->getParam(PARAM_INPUT_2)->value();
      Serial.printf("apiKey: %s/n", apiKey);
      if(apiKey != ""){
      this->htmlHandler->writeFile(SD, "/api_key.txt", apiKey.c_str());
      }
    }
    request->redirect("/Server");
  });

  _server->on("/log", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/log.txt", String(), false, this->htmlHandler->processor);
  });

  _server->on("/api/download", HTTP_GET, [this](AsyncWebServerRequest *request){
    const char* dataType = "text/plain"; 
    AsyncWebServerResponse *response = request->beginResponse(SD, "/log.txt", dataType, true);
    request->send(response);
  });

  _server->on("/api/downloadraw", HTTP_GET, [this](AsyncWebServerRequest *request){
        const char* dataType = "text/plain"; 
        AsyncWebServerResponse *response = request->beginResponse(SD, "/dataraw.csv", dataType, true);
        request->send(response);
  });


  _server->on("/", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/index.html", String(), false, this->htmlHandler->processor);
  });
  
  _server->on("/Server", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Server.html", String(), false, this->htmlHandler->processor);
  });

  _server->on("/Data", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Data.html", String(), false, this->htmlHandler->processor);
  });

  // _server->on("/Chart", HTTP_GET, [this] (AsyncWebServerRequest *request) {
  //   request->send(SD, "/Chart.html", String(), false, this->htmlHandler->processor);
  // });

  _server->on("/Configuration", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Configuration.html", String(), false, this->htmlHandler->processor);
  });

  _server->on("/Sensor1", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Calibration.html", String(), false, this->htmlHandler->processor);
  });
  _server->on("/Sensor2", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Sensor2.html", String(), false, this->htmlHandler->processor);
  });
  _server->on("/Sensor3", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Sensor3.html", String(), false, this->htmlHandler->processor);
  });
  _server->on("/Sensor4", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Sensor4.html", String(), false, this->htmlHandler->processor);
  });
  _server->on("/Sensor5", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    request->send(SD, "/Sensor5.html", String(), false, this->htmlHandler->processor);
  });
  
  _server->on("/s1", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readSens1().c_str());
  });
  _server->on("/s2", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readSens2().c_str());
  });
  _server->on("/s3", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readSens3().c_str());
  });
  _server->on("/s4", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readSens4().c_str());
  });
  _server->on("/s5", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readSens5().c_str());
  });
  
  _server->on("/voltage", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readVolt().c_str());
  });

  _server->on("/daya", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readDAYA().c_str());
  });

  _server->on("/rssi", HTTP_GET, [this](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", this->htmlHandler->readRSSI().c_str());
  });

  _server->on("/config", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    Serial.print("Content type::");
    Serial.println(request->contentType());
    Serial.println("OFF hit.");

    int param = request->params();
    Serial.printf("%d params sent in\n", param);
      
    String durasisleep;
    String password;
    String tinggiair;
    String PARAM_INPUT_SLEEP = "durasisleep";
    String PARAM_INPUT_PASSWORD = "password";
    String PARAM_INPUT_TINGGI = "tinggiair";
    String quicksend;
    String PARAM_INPUT_QUICKSEND = "quicksend";
    
    // // GET input1 value on <ESP_IP>/update?output=<endpoint>&state=<apiKey>

    if (request->hasParam(PARAM_INPUT_SLEEP)){
      durasisleep = request->getParam(PARAM_INPUT_SLEEP)->value();
      Serial.printf("durasisleep: %s/n", durasisleep);
      if(durasisleep != ""){
      this->htmlHandler->writeFile(SD, "/durasi_sleep.txt", durasisleep.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_PASSWORD)){
      password = request->getParam(PARAM_INPUT_PASSWORD)->value();
      Serial.printf("password: %s/n", password);
      if(password != ""){
      this->htmlHandler->writeFile(SD, "/pass_word.txt", password.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_TINGGI)){
      tinggiair = request->getParam(PARAM_INPUT_TINGGI)->value();
      Serial.printf("tinggiair: %s/n", tinggiair);
      if(tinggiair != ""){
      this->htmlHandler->writeFile(SD, "/tinggi.txt", tinggiair.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_QUICKSEND)){
      quicksend = request->getParam(PARAM_INPUT_QUICKSEND)->value();
      Serial.printf("quicksend: %s/n", quicksend);
      this->htmlHandler->writeFile(SD, "/quicksend.txt", quicksend.c_str());
    }

    request->redirect("/Configuration");
  });

  _server->on("/cal_sensor", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    Serial.print("Content type::");
    Serial.println(request->contentType());
    Serial.println("OFF hit.");

    int params2 = request->params();
    Serial.printf("%d params sent in\n", params2);
      
    String a1;
    String c1;
    String PARAM_INPUT_19 = "a1";
    String PARAM_INPUT_20 = "c1";
    
    // // GET input1 value on <ESP_IP>/update?output=<endpoint>&state=<apiKey>

    if (request->hasParam(PARAM_INPUT_19)){
      a1 = request->getParam(PARAM_INPUT_19)->value();
      Serial.printf("a1: %s/n", a1);
      if(a1 != ""){
      this->htmlHandler->writeFile(SD, "/a_1.txt", a1.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_20)){
      c1 = request->getParam(PARAM_INPUT_20)->value();
      Serial.printf("c1: %s/n", c1);
      if(c1 != ""){
      this->htmlHandler->writeFile(SD, "/c_1.txt", c1.c_str());
      }
    }

    // String a2;
    // String c2;
    // String PARAM_INPUT_3 = "a2";
    // String PARAM_INPUT_4 = "c2";
    
    
    // if (request->hasParam(PARAM_INPUT_3)){
    //   a2 = request->getParam(PARAM_INPUT_3)->value();
    //   Serial.printf("a2: %s/n", a2);
    //   if(a2 != ""){
    //   this->htmlHandler->writeFile(SD, "/a_2.txt", a2.c_str());
    //   }
    // }

    // if (request->hasParam(PARAM_INPUT_4)){
    //   c2 = request->getParam(PARAM_INPUT_4)->value();
    //   Serial.printf("c2: %s/n", c2);
    //   if(c2 != ""){
    //   this->htmlHandler->writeFile(SD, "/c_2.txt", c2.c_str());
    //   }
    // }

    
  
    // String a3;
    // String c3;
    // String PARAM_INPUT_5 = "a3";
    // String PARAM_INPUT_6 = "c3";
    
    
    // if (request->hasParam(PARAM_INPUT_5)){
    //   a3 = request->getParam(PARAM_INPUT_5)->value();
    //   Serial.printf("a3: %s/n", a3);
    //   if(a3 != ""){
    //   this->htmlHandler->writeFile(SD, "/a_3.txt", a3.c_str());
    //   }
    // }

    // if (request->hasParam(PARAM_INPUT_6)){
    //   c3 = request->getParam(PARAM_INPUT_6)->value();
    //   Serial.printf("c3: %s/n", c3);
    //   if(c3 != ""){
    //   this->htmlHandler->writeFile(SD, "/c_3.txt", c3.c_str());
    //   }
    // }

    // String a4;
    // String c4;
    // String PARAM_INPUT_7 = "a4";
    // String PARAM_INPUT_8 = "c4";
    
    
    // if (request->hasParam(PARAM_INPUT_7)){
    //   a4 = request->getParam(PARAM_INPUT_7)->value();
    //   Serial.printf("a4: %s/n", a4);
    //   if(a4 != ""){
    //   this->htmlHandler->writeFile(SD, "/a_4.txt", a4.c_str());
    //   }
    // }

    // if (request->hasParam(PARAM_INPUT_8)){
    //   c4 = request->getParam(PARAM_INPUT_8)->value();
    //   Serial.printf("c4: %s/n", c4);
    //   if(c4 != ""){
    //   this->htmlHandler->writeFile(SD, "/c_4.txt", c4.c_str());
    //   }
    // }

    // String a5;
    // String c5;
    // String PARAM_INPUT_9 = "a5";
    // String PARAM_INPUT_10 = "c5";
    
    
    // if (request->hasParam(PARAM_INPUT_9)){
    //   a5 = request->getParam(PARAM_INPUT_9)->value();
    //   Serial.printf("a5: %s/n", a5);
    //   if(a5 != ""){
    //   this->htmlHandler->writeFile(SD, "/a_5.txt", a5.c_str());
    //   }
    // }

    // if (request->hasParam(PARAM_INPUT_10)){
    //   c5 = request->getParam(PARAM_INPUT_10)->value();
    //   Serial.printf("c5: %s/n", c5);
    //   if(c5 != ""){
    //   this->htmlHandler->writeFile(SD, "/c_5.txt", c5.c_str());
    //   }
    // }

    request->redirect("/Sensor1");
  });




  _server->on("/cal_sensor2", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    Serial.print("Content type::");
    Serial.println(request->contentType());
    Serial.println("OFF hit.");

    int params3 = request->params();
    Serial.printf("%d params sent in\n", params3);

    String a2;
    String c2;
    String PARAM_INPUT_3 = "a2";
    String PARAM_INPUT_4 = "c2";
    
    
    if (request->hasParam(PARAM_INPUT_3)){
      a2 = request->getParam(PARAM_INPUT_3)->value();
      Serial.printf("a2: %s/n", a2);
      if(a2 != ""){
      this->htmlHandler->writeFile(SD, "/a_2.txt", a2.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_4)){
      c2 = request->getParam(PARAM_INPUT_4)->value();
      Serial.printf("c2: %s/n", c2);
      if(c2 != ""){
      this->htmlHandler->writeFile(SD, "/c_2.txt", c2.c_str());
      }
    }  
    

    request->redirect("/Sensor2");
  });
  
  _server->on("/cal_sensor3", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    Serial.print("Content type::");
    Serial.println(request->contentType());
    Serial.println("OFF hit.");

    int params4 = request->params();
    Serial.printf("%d params sent in\n", params4);

    String a3;
    String c3;
    String PARAM_INPUT_5 = "a3";
    String PARAM_INPUT_6 = "c3";
    
    
    if (request->hasParam(PARAM_INPUT_5)){
      a3 = request->getParam(PARAM_INPUT_5)->value();
      Serial.printf("a3: %s/n", a3);
      if(a3 != ""){
      this->htmlHandler->writeFile(SD, "/a_3.txt", a3.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_6)){
      c3 = request->getParam(PARAM_INPUT_6)->value();
      Serial.printf("c3: %s/n", c3);
      if(c3 != ""){
      this->htmlHandler->writeFile(SD, "/c_3.txt", c3.c_str());
      }
    }  
    

    request->redirect("/Sensor3");
  });

  _server->on("/cal_sensor4", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    Serial.print("Content type::");
    Serial.println(request->contentType());
    Serial.println("OFF hit.");

    int params5 = request->params();
    Serial.printf("%d params sent in\n", params5);

    String a4;
    String c4;
    String PARAM_INPUT_7 = "a4";
    String PARAM_INPUT_8 = "c4";
    
    
    if (request->hasParam(PARAM_INPUT_7)){
      a4 = request->getParam(PARAM_INPUT_7)->value();
      Serial.printf("a4: %s/n", a4);
      if(a4 != ""){
      this->htmlHandler->writeFile(SD, "/a_4.txt", a4.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_8)){
      c4 = request->getParam(PARAM_INPUT_8)->value();
      Serial.printf("c4: %s/n", c4);
      if(c4 != ""){
      this->htmlHandler->writeFile(SD, "/c_4.txt", c4.c_str());
      }
    }  
    

    request->redirect("/Sensor4");
  });

  _server->on("/cal_sensor5", HTTP_GET, [this] (AsyncWebServerRequest *request) {
    Serial.print("Content type::");
    Serial.println(request->contentType());
    Serial.println("OFF hit.");

    int params6 = request->params();
    Serial.printf("%d params sent in\n", params6);
      
    String a5;
    String c5;
    String PARAM_INPUT_9 = "a5";
    String PARAM_INPUT_10 = "c5";
    
    
    if (request->hasParam(PARAM_INPUT_9)){
      a5 = request->getParam(PARAM_INPUT_9)->value();
      Serial.printf("a5: %s/n", a5);
      if(a5 != ""){
      this->htmlHandler->writeFile(SD, "/a_5.txt", a5.c_str());
      }
    }

    if (request->hasParam(PARAM_INPUT_10)){
      c5 = request->getParam(PARAM_INPUT_10)->value();
      Serial.printf("c5: %s/n", c5);
      if(c5 != ""){
      this->htmlHandler->writeFile(SD, "/c_5.txt", c5.c_str());
      }
    }
    

    request->redirect("/Sensor5");
  });

  

  _server->serveStatic("/", SD, "/");
  


  // Websocket Callback Handler
  


}

