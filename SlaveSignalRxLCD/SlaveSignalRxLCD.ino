// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX
#include <SPI.h>
#include <RH_RF95.h>
#include <elapsedMillis.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 433.0
#define TX_POWER_LEVEL 23
// power and state indicator
#define LED_POWER 13
#define WAIT_TIME 5000 // 20 seconds
#define STATE_IDLE 0xA0
#define STATE_REQUEST_SIGNAL_VALID 0xA1
#define STATE_TIMER_COUNT 0xA2
unsigned long currentState = STATE_IDLE;
char* requestRxToken;
const char RESPONSE_TOKEN[20] = "1234567890123456789";
const char REQUEST_TOKEN[20] = "ABCDEFGH1JKLMNOPQRS";
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
LiquidCrystal_I2C lcd(0x3f,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
elapsedMillis waitingTimeElapsed;
uint8_t c = 0;
void clearlcd(){
  lcd.init();
  lcd.backlight();
}
void countlcd(){
  lcd.setCursor(9,2);
  lcd.print(++c);   
} 
void text(char txt[], uint8_t col = 0, uint8_t row = 0, boolean doclear = false){
  if (doclear){
    clearlcd();
  }
  lcd.setCursor(col,row);
  lcd.print(txt);
}
void setup() {
  pinMode(LED_POWER, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  // while (!Serial); uncomment when debugging
  Serial.begin(9600);
  delay(100);
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
    out("LoRa radio init failed");
    while (1);
  }
  out("LoRa radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    out("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(TX_POWER_LEVEL, false);  
  text("*** Welcome ***",2,0, true);
  text(">Waiting for signal<",0,1);
}
void loop() {
  while(currentState == STATE_IDLE){
      // out("idle state");
      if (rf95.available()){
        // Should be a message for us now   
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);      
        if (rf95.recv(buf, &len)) {
          digitalWrite(LED_POWER, HIGH);
          RH_RF95::printBuffer("Received: ", buf, len);
          Serial.print("Got: ");
          requestRxToken = (char*)buf;
          out(requestRxToken);
          // compare token if valid
          if (strcmp(requestRxToken, REQUEST_TOKEN) == 0){
            Serial.print("RSSI: ");
            Serial.print(rf95.lastRssi(), DEC);
            delay(10);
            // Send a reply
            delay(200); // may or may not be needed
            // replay validation message
            uint8_t data[] = "1234567890123456789";
            rf95.send(data, sizeof(data));
            rf95.waitPacketSent();
            out("Sent a reply");
            digitalWrite(LED_POWER, LOW);
            out(">> VALIDATION SENT BACK");
            text("> Signal found <",2,0,true);
            text(" Waiting ",5,1);
            currentState = STATE_REQUEST_SIGNAL_VALID;waitingTimeElapsed = 0; c = 0;
          } else {
            out("invalid token");
          }
        } else {
          out("Receive failed");
        }
    }
  }
  while(currentState == STATE_REQUEST_SIGNAL_VALID){
    // blink LED blink here; start time elapse count
    // delay accounts for doors and bus proceeding to next stop; ignores other signals for 5 secs
    blinkState();delay(500);blinkState();delay(500);countlcd();
    if (waitingTimeElapsed > WAIT_TIME){
      digitalWrite(LED_POWER, HIGH);
      text("Doors are closing",1,0, true);
      text(">                  <",0,1);delay(2000);
      text(">>>              <<<",0,1);delay(500);
      text(">>>>>          <<<<<",0,1);delay(500);
      text(">>>>>>>>    <<<<<<<<",0,1);delay(500);
      text(">>>>>>>>>><<<<<<<<<<",0,1);delay(500);
      // waiting time; reset to idle state; reset the counter to 0 so the counting starts over...
      waitingTimeElapsed = 0; c = 0; currentState = STATE_IDLE;
      text("*** Welcome ***",2,0, true);
      text(">Waiting for signal<",0,1);
      delay(10000);
    }
  }
}
void out(String v){
  Serial.println(v);
}
void blinkState(){
  uint8_t ledState = digitalRead(LED_POWER);  // read input value
  ledState = (ledState == HIGH) ? LOW : HIGH;
  digitalWrite(LED_POWER, ledState);
}

