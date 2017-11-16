#include "Adafruit_FONA.h"

#define FONA_RST 4
#define PUSH_BUTTON 2
#define OK_REPLY "OK"

// Hardware serial is also possible!
HardwareSerial *fonaSerial = &Serial1;
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);
uint8_t type;
char replybuffer[255];
boolean validgps = false;
int pv = 0;

void setup() {
  while (!Serial);
  pinMode(PUSH_BUTTON, INPUT); 
  Serial.begin(4800);  
  Serial.println(F("FONA 3G Setup...")); //Serial.println(F("FONA basic test"));Serial.println(F("Initializing....(May take 3 seconds)"));
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {Serial.println(F("Couldn't find FONA"));return;}
  type = fona.type();
  if (type == 5){
    Serial.println(F("FONA is OK -> FONA 3G (European); ready to start GPRSN..."));
    sendCmd("AT+CHTTPSSTOP");delay(500);
    fona.setGPRSNetworkSettings(F("e-ideas"), F(""), F(""));
    delay(2000);
    enableGPRS();
    delay(2000);
    sendCmd("AT+CHTTPSSTART");
    delay(2000);
    openURL();
    delay(2000);
    postCoordinates();
    delay(2000);
    //readPostResponse();
  }else{
    Serial.println(F("Invalid FONA 3G (European)"));return;
  }  
}
boolean sendCmd(char *send,uint16_t timeout){
  return fona.sendCheckReply(send, OK_REPLY, timeout);
}
boolean sendCmd(char *send){
  return fona.sendCheckReply(send, OK_REPLY, 1000);
}
boolean sendCmd(char *send, char *reply){
  return fona.sendCheckReply(send, reply);
}
void sendFonaString(char *send){
  Serial.println(send);
  fonaSerial->println(send);delay(100);readline(strlen(send)+2, false);
}
void readline(uint16_t timeout, boolean multiline) {
  uint16_t replyidx = 0;  
  while (timeout--) {
    if (replyidx >= sizeof(replybuffer)-1) {break;}
    while(fonaSerial->available()) {
      char c =  fonaSerial->read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0) continue;   // the first 0x0A is ignored
        if (!multiline) {timeout = 0;break;}// the second 0x0A is the end of the line
      }
      replybuffer[replyidx] = c;replyidx++;
    }
    if (timeout == 0) {break;}
    delay(1);
  } 
  replybuffer[replyidx] = 0;
}
void readPostResponse(){
  float state;
  Serial.println("Post complete -> HTTP SEND... -> read response...");
  if (fona.sendParseReply(F("AT+CHTTPSRECV?"), F("+CHTTPSRECV: LEN,"), &state, ',', 0)){  
    int l = 0;
    validgps = false;
    while (fona.sendParseReply(F("AT+CHTTPSRECV=289"), F("OK"), &state, ',', 0)){
      readline(1000, true);size_t sz = strlen(replybuffer);
      if (!validgps)
        validgps = strlen(strstr(replybuffer, "\"valid\":true")) > 0;
      Serial.println("state --> ");Serial.println(state);
      Serial.println("buffer --> ");Serial.println(replybuffer);
      if (strlen(strstr(replybuffer, "Connection: Close")) > 0) break;
      if (strlen(strstr(replybuffer, "Connection: keep-alive")) > 0) break;
      if ((sz == 0 && ++l > 2) || validgps) break;
    }
    Serial.print("valid gps -> ");Serial.println(validgps);
    Serial.println("read done");
  }
}
boolean checkReplyBuffer(char *reply){
  return (strcmp(replybuffer, reply) == 0);
}
void postCoordinates(){
  Serial.println(F("Http send -> post parameters"));delay(1000);
  if (sendCmd("AT+CHTTPSSEND=253", ">")){      
      sendFonaString("POST /Validate?lon=1.111111&lat=1.222211 HTTP/1.1");sendFonaString("Host: default-environment.rezn3yycxc.us-east-1.elasticbeanstalk.com");
      sendFonaString("Connection: keep-alive");sendFonaString("Content-Encoding: gzip");
      sendFonaString("Server: Apache-Coyote/1.1");sendFonaString("Content-Type: application/json;charset=UTF-8");
      sendFonaString("Vary: Accept-Encoding");//Serial.println("waiting ok reply");
      sendFonaString("AT+CHTTPSSEND");
      while (!checkReplyBuffer("OK")){sendFonaString("AT+CHTTPSSEND");} 
      //readPostResponse();
  }else{Serial.println(F("Http send fail"));openURL();}
}
void openURL(){
  delay(500);
  if (sendCmd("AT+CHTTPSOPSE=\"default-environment.rezn3yycxc.us-east-1.elasticbeanstalk.com\",80,1", 3000)){
    Serial.println(F("API opened -> Prepare HTTP Send"));
    //postCoordinates();
  }else{Serial.println(F("API fail to open"));}
  //sendCmd("AT+CHTTPSCLSE");
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
  delay(1);
  fona.setGPRSNetworkSettings(F("e-ideas"), F(""), F(""));
  delay(500);
  enableGPRS();
}

void enableGPRS(){
  delay(3000);
  fona.enableGPRS(false);
  delay(3000);
  fona.enableGPRS(true);
  delay(3000);
  /*if (!n){    
    Serial.println(F("GPRS Failed to turn on -> re-try GPRS to turn on..."));delay(500);
    
    delay(1000);
    flushSerial();
    fona.enableGPRS(false);
    delay(1000);
    flushSerial();
    enableGPRS();
  }else{
    Serial.println(F("GPRS turned on"));setGPSValidate();
  }
  */
}

void flushSerial() {
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
      while(fonaSerial->available()) {
        fonaSerial->read();timeoutloop = 0;  // If char was received reset the timer
      }delay(1);
    }
}
void loop() {
  if (Serial.available()){fonaSerial->write(Serial.read());}
  if (fonaSerial->available()){Serial.write(fonaSerial->read());}
  pv = digitalRead(PUSH_BUTTON);
  if (pv == HIGH){
    setup();
  }
  /*if (Serial.available()){
    char c = Serial.read();
    if (c == 'r'){
      Serial.println("post data");
      postCoordinates();
    }
    if (c == 'g'){
      Serial.println("set gprsn");
      setGPRSN();
    }
  }*/
}

