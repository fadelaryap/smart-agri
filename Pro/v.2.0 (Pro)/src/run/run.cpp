#include "run.h"

// #include "Declare.h"




File myfile;
sd_card::sd_card(int _serverURI, int _key){
    this->serverURI = _serverURI;
    this->key = _key;
}

void sd_card::initSDCard(){
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }
  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

//--Menambah data ke file SD Card--//
void sd_card::appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  myfile = fs.open(path, FILE_APPEND);
  if(!myfile) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(myfile.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  myfile.close();
}

void sd_card::writeFile(fs::FS &fs, const char *path, const char *message)
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

void sd_card::cekfiledataraw(){
  myfile = SD.open("/dataraw.csv");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/dataraw.csv",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
}

void sd_card::cekfile3(){
  myfile = SD.open("/RSSI.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/RSSI.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
  
  myfile.close();
}

void sd_card::ceksleepdanpassword(){
  myfile = SD.open("/durasi_sleep.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/durasi_sleep.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
  myfile.close();
  delay(1000);
  myfile = SD.open("/pass_word.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/pass_word.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
  myfile.close();
  delay(1000);
}

void sd_card::cekfile2(){
  for(int j = 1;j<8;j++) {
    char baf[20];
    sprintf(baf, "/SENS%d.txt",j);
    myfile = SD.open(baf);
    if(!myfile) {
      Serial.println("File doesn't exist");
      Serial.println("Creating file...");
      myfile = SD.open(baf,FILE_WRITE);
    }
    else {
      Serial.println("File already exists");  
    }
  }
  myfile.close();
}


void sd_card::cekfile(){
  myfile = SD.open("/datasensor.txt");
  if(!myfile) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    myfile = SD.open("/datasensor.txt",FILE_WRITE);
  }
  else {
    Serial.println("File already exists");  
  }
}


// void sendToServer3(){
//   String requestData = "&C0=30.00&C1=" + String(result[3]) + "&C2=" + String(result[4]) + "&C3=" + String(result[5]) + "&C4=" + String(result[6]) + "&C5=" + String(result[7]) + "&C6=" + String(result[8]);
//   Serial.println(requestData); 
//   Serial.println("Performing HTTP POST request...");
//   client2.print("GET /feed/api3.php?K=");
//   client2.print("87bb155481f2d8308cff4a2e247b5ce5");
//   client2.print(requestData);
//   client2.print(" HTTP/1.0\n");
//   client2.print("Host: ");
//   client2.println(server);
//   client2.print("User-Agent: Arduino-agrieye\n");
//   client2.println();

//   unsigned long timeout = millis();
//   while (client2.connected() && millis() - timeout < 10000L) {
//     // Print available data (HTTP response from server)
//     while (client2.available()) {
//       char c = client2.read();
//       Serial.print(c);
//       timeout = millis();
//     }
//   }
// }