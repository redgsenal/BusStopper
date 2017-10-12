#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define OK_REPLY "OK"

#include <SoftwareSerial.h>
#include <Wire.h> 

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);
uint8_t type;
char replybuffer[255];

void setup() {
  while (!Serial);
  Serial.begin(4800);  
  Serial.println(F("FONA 3G Setup..."));
  Serial.println(F("FONA basic test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  type = fona.type();
  if (type == 5){
    Serial.println(F("FONA is OK"));
    Serial.println(F("FONA 3G (European)"));
    Serial.println("ready to start GPRSN...");
    setGPRSN();
  }else{
    Serial.println(F("Invalid FONA 3G (European)"));
    while(1);
  }  
}
boolean sendCmd(char *send){
  return fona.sendCheckReply(send, OK_REPLY);
}
boolean sendCmd(char *send, char *reply){
  return fona.sendCheckReply(send, reply);
}
void sendFonaString(char *send){
  Serial.println(send);  
  fonaSS.println(send);
  readline(100, false);
}
void readline(uint16_t timeout, boolean multiline) {
  uint16_t replyidx = 0;  
  while (timeout--) {
    if (replyidx >= 254) {break;}
    while(fonaSS.available()) {
      char c =  fonaSS.read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0) continue;   // the first 0x0A is ignored
        if (!multiline) {
          timeout = 0;         // the second 0x0A is the end of the line
          break;
        }
      }
      replybuffer[replyidx] = c;replyidx++;
    }
    if (timeout == 0) {
      //DEBUG_PRINTLN(F("TIMEOUT"));
      break;
    }
    delay(1);
  } 
  replybuffer[replyidx] = 0;  
}

boolean checkReplyBuffer(char *reply){
  return (strcmp(replybuffer, reply) == 0);
}
void readPostResponse(){
  while (!checkReplyBuffer("OK")){sendFonaString("AT+CHTTPSSEND");}
  Serial.println("Post complete -> HTTP SEND... -> read response...");
  if (fona.sendParseReply(F("AT+CHTTPSRECV?"), F("+CHTTPSRECV: LEN,"), ',', 0)){
    while (fona.sendParseReply(F("AT+CHTTPSRECV=512"), F("OK"), ',', 0)){
      readline(5000, true);
      Serial.println("buffer-->");
      Serial.println(replybuffer);
      size_t sz = strlen(replybuffer);
      Serial.println(sz);
      if (sz == 0)
        break;
      else{
        if (sz > 20){
          boolean validgps = strlen(strstr(replybuffer, "\"valid\":true")) > 0;
          Serial.println("gps validate here");
          Serial.println(validgps);
        }// else skip other responses and keep reading...
      }
    }
    Serial.println("read done");
  }
}
void readFonaSerial(int16_t length){
  while (length > 0) {
    while (fona.available()) {
      Serial.println("-->");
      Serial.println(length);
      Serial.write(fona.read());
      length--;
      if (!length) break;
    }
  }
}
void openURL(){
  delay(500);flushSerial();
  if (sendCmd("AT+CHTTPSOPSE=\"default-environment.rezn3yycxc.us-east-1.elasticbeanstalk.com\",80,1")){
    Serial.println(F("API opened -> Prepare HTTP Send"));
    if (sendCmd("AT+CHTTPSSEND=512", ">")){
      Serial.println(F("Http send -> post parameters"));delay(100);
      sendFonaString("POST /Validate?lon=1.111111&lat=1.222211 HTTP/1.1");
      sendFonaString("Host: default-environment.rezn3yycxc.us-east-1.elasticbeanstalk.com");
      sendFonaString("Connection: keep-alive");
      sendFonaString("Content-Encoding: gzip");
      sendFonaString("Server: Apache-Coyote/1.1");
      sendFonaString("Content-Type: application/json;charset=UTF-8");
      sendFonaString("Vary: Accept-Encoding");
      //sendFonaString("Content-Length: 0");
      Serial.println("waiting ok reply"); 
      readPostResponse();
      return;
    }else{Serial.println(F("Http send fail"));}
  }else{Serial.println(F("API fail to open"));sendCmd("AT+CHTTPSCLSE");}
}

void setGPSValidate(){  
  Serial.println("post sample data");
  if (sendCmd("AT+CHTTPSSTART")){Serial.println(F("HTTP start"));openURL();
  }else{
    Serial.println(F("HTTP failed to start - > restart HTTP start"));setGPRSN();
  }
}

void setGPRSN(){
  Serial.println(F("Starting GPRS..."));
  // M1 setup//fona.setGPRSNetworkSettings(F("sunsurf"), F("65"), F("user123"));
  // Singtel
  delay(5000);
  fona.setGPRSNetworkSettings(F("e-ideas"), F(""), F(""));
  if (!fona.enableGPRS(true)){
    Serial.println(F("GPRS Failed to turn on -> re-try GPRS to turn on..."));delay(3000);
    fona.enableGPRS(false);delay(5000);
    setGPRSN();
  }else{
    Serial.println(F("GPRS turned on"));setGPSValidate();
  }
}

void flushSerial() {
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
      while(fonaSS.available()) {
        fonaSS.read();timeoutloop = 0;  // If char was received reset the timer
      }delay(1);
    }
}

void loop() {
  if (fonaSS.available()){Serial.write(fonaSS.read());}
  if (Serial.available()){fonaSS.write(Serial.read());}  
}

