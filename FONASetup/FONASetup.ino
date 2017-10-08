#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

#include <SoftwareSerial.h>
#include <Wire.h> 

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t type;

void setup() {
  while (!Serial);
  Serial.begin(115200);
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
  }else{
    Serial.println(F("Invalid FONA 3G (European)"));
    while(1);
  }

  // Print module IMEI number.
  char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI: "); Serial.println(imei);
  }
  // setup M1 setup
  fona.setGPRSNetworkSettings(F("sunsurf"), F("65"), F("user123"));
  delay(1000);
  fona.enableGPRS(true);
  delay(1000);
  fona.enableGPS(true);  
  Serial.println(F("FONA 3G setup done"));
  Serial.println(F("Wating for Serial AT Commands..."));
  sendATCommand("AT", "OK");
  sendATCommand("AT+CGPS?", "OK");
  postSampleData();
}

void postSampleData(){
  Serial.println(F("Psoting sample data"));
  if (!fona.enableGPRS(true)){
    Serial.println(F("Failed to turn on"));
    return;
  }
  delay(5000);
  Serial.println("post sample data");
  // Post data to website
  uint16_t statuscode;
  int16_t length;
  char *url = "http://localhost:8080/Validate/GPS?gps";
  char *data = "{'key': 1, 'lon':1, 'lat': 2}";
  if (!fona.HTTP_POST_start(url, F("text/plain"), (uint8_t *) data, strlen(data), &statuscode, (uint16_t *)&length)) {
    Serial.println("Failed!");
    return;
  }
  while (length > 0) {
    while (fona.available()) {
      char c = fona.read();
      #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
        UDR0 = c;
      #else
        Serial.write(c);
      #endif
      length--;
      if (! length) break;
    }
  }
  Serial.println(F("\n****"));
  fona.HTTP_POST_end();        
}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}

void sendATCommand(char *at, char *rply){
  delay(1000);
  Serial.println("send: ");
  Serial.println(at);
  delay(1);
  fona.sendCheckReply(at, rply);
  while (fona.available()) {
    Serial.write(fona.read());
  }
}

void loop() {  
}
