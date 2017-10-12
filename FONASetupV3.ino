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
    delay(5000);  
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
    if (replyidx >= 254) {
      //DEBUG_PRINTLN(F("SPACE"));
      break;
    }
    while(fonaSS.available()) {
      char c =  fonaSS.read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;

        if (!multiline) {
          timeout = 0;         // the second 0x0A is the end of the line
          break;
        }
      }
      Serial.println(c);
      replybuffer[replyidx] = c;
      replyidx++;
    }
    if (timeout == 0) {
      //DEBUG_PRINTLN(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  Serial.println("reply buffer ->");
  Serial.println(replybuffer);
}

boolean checkReplyBuffer(char *reply){
  return (strcmp(replybuffer, reply) == 0);
}

void openURL(){
  delay(1000);
  flushSerial();
  if (sendCmd("AT+CHTTPSOPSE=\"default-environment.rezn3yycxc.us-east-1.elasticbeanstalk.com\",80,1")){
    Serial.println(F("API opened -> Prepare HTTP Send"));
    if (sendCmd("AT+CHTTPSSEND=512", ">")){
      Serial.println(F("Http send -> post parameters"));
      delay(100);
      // setup the POST/GET header 
      sendFonaString("POST /Validate?lon=1.111111&lat=1.222211 HTTP/1.1");
      sendFonaString("Host: default-environment.rezn3yycxc.us-east-1.elasticbeanstalk.com");
      sendFonaString("Connection: keep-alive");
      sendFonaString("Content-Encoding: gzip");
      sendFonaString("Server: Apache-Coyote/1.1");
      sendFonaString("Content-Type: application/json;charset=UTF-8");
      sendFonaString("Vary: Accept-Encoding");
      //sendFonaString("Content-Length: 0");
      Serial.println("waiting ok reply"); 
      while (!checkReplyBuffer("OK")){
        sendFonaString("AT+CHTTPSSEND");
      }
      delay(500);
      Serial.println("Post complete -> HTTP SEND...");          
      Serial.println("read response...");
      sendFonaString("AT+CHTTPSRECV=300");
      return;
    }else{
      Serial.println(F("Http send fail"));
    }
  }else{
    Serial.println(F("API fail to open"));
      sendCmd("AT+CHTTPSCLSE");
  }
}

void setGPSValidate(){
  Serial.println(F("GPRS turned on"));    
  Serial.println("post sample data");
  // enable http start
  if (sendCmd("AT+CHTTPSSTART")){
      Serial.println(F("HTTP start"));
      openURL();
    }else{
      Serial.println(F("HTTP failed to start"));
    } 
}

void setGPRSN(){
  Serial.println(F("Starting GPRS..."));
  // M1 setup
  //fona.setGPRSNetworkSettings(F("sunsurf"), F("65"), F("user123"));
  // Singtel
  fona.setGPRSNetworkSettings(F("e-ideas"), F(""), F(""));
  if (!fona.enableGPRS(true)){
    Serial.println(F("GPRS Failed to turn on"));
    Serial.println(F("re-try GPRS to turn on..."));
    delay(3000);
    fona.enableGPRS(false);
    delay(5000);
    setGPRSN();
  }else{
    setGPSValidate();
  }
}

void flushSerial() {
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
      while(fonaSS.available()) {
        fonaSS.read();
        timeoutloop = 0;  // If char was received reset the timer
      }
      delay(1);
    }
}

void loop() {
  if (fonaSS.available()){
    Serial.write(fonaSS.read());
  }
  if (Serial.available()){
      fonaSS.write(Serial.read());
  }  
}

