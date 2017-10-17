#include <SPI.h>
#include <RH_RF95.h>
#include <elapsedMillis.h>
/* for feather m0 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 433.0
// D6
#define CALL_BUTTON 6
// D10 - BUTTON LED
#define LED_INDICATOR 10
// D13 - ON LED
#define LED_POWER 13
#define STATE_IDLE 0xA0
#define STATE_BUTTON_PRESSED_SEND_REQUEST 0xA1
#define STATE_RESPONSE_RECEIVED 0xA2
#define REQUEST_TIME 20000 // waiting to time-out 
#define WAIT_TIME 20000 // 20 seconds
#define TX_POWER_LEVEL 23
const char RESPONSE_TOKEN[20] = "1234567890123456789";
const char REQUEST_TOKEN[20] = "ABCDEFGH1JKLMNOPQRS";
unsigned long currentState = STATE_IDLE;
uint8_t callButtonState = 0;
char* requestRxToken;
RH_RF95 rf95(RFM95_CS, RFM95_INT);
elapsedMillis waitingTimeElapsed;
int16_t packetnum = 0;  // packet counter, we increment per xmission
void blinkState(unsigned long int d = 500){
  int ledState = digitalRead(LED_POWER);  // read input value
  ledState = (ledState == HIGH) ? LOW : HIGH;
  digitalWrite(LED_POWER, ledState);
  delay(d);
}
void out(String v){
  Serial.println(v);
}
int checkButton(){
  int ledState = digitalRead(CALL_BUTTON);  // read input value
  digitalWrite(LED_INDICATOR, ledState);  // turn LED OFF
  return ledState;
}
void sendRequest() {
  Serial.println("Sending to rf95_server");
  // Send a message to rf95_server
  
  char radiopacket[] = "ABCDEFGH1JKLMNOPQRS"; //"Hello World #      ";
  //itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Sending "); Serial.println(radiopacket);
  radiopacket[19] = 0;
  
  Serial.println("Sending..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 20);

  Serial.println("Waiting for packet to complete..."); delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len)) {
      Serial.print("Got reply: ");
      requestRxToken = (char*)buf;
      Serial.println(requestRxToken);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      if (strcmp(requestRxToken, RESPONSE_TOKEN) == 0){  
        currentState = STATE_RESPONSE_RECEIVED;
        waitingTimeElapsed = 0;
        out("valid token");
      }else{
        out("invalid token");
      }
    } else {
      Serial.println("Receive failed");
    }
  } else {
    Serial.println("No reply, is there a listener around?");
  }  
}
void setup() {
  pinMode(LED_POWER, OUTPUT);  // declare LED as output
  digitalWrite(LED_POWER, HIGH);
  pinMode(LED_INDICATOR, OUTPUT);  // declare LED as output
  pinMode(CALL_BUTTON, INPUT);    // declare pushbutton as input
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
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(TX_POWER_LEVEL, false);
}
void loop() {
  // loop here on button states
  checkButton();
  while(currentState == STATE_IDLE){
    out("waiting button");
    callButtonState = checkButton();
    if (callButtonState == HIGH){
      out("button pressed");
      currentState = STATE_BUTTON_PRESSED_SEND_REQUEST;
      waitingTimeElapsed = 0;
    }    
  }

  while(currentState == STATE_BUTTON_PRESSED_SEND_REQUEST){
    out("button pressed; send signal");
    blinkState(50);
    sendRequest();
    if (waitingTimeElapsed > REQUEST_TIME){
      digitalWrite(LED_INDICATOR, LOW);
      digitalWrite(LED_POWER, HIGH);
      currentState = STATE_IDLE;
      waitingTimeElapsed = 0;
      out("request time out; reset to idle");
    }
  }

  while(currentState == STATE_RESPONSE_RECEIVED){
    blinkState(200);
    digitalWrite(LED_INDICATOR, HIGH);
    if (waitingTimeElapsed > WAIT_TIME){
      digitalWrite(LED_POWER, HIGH);
      // waiting time; reset to idle state
      waitingTimeElapsed = 0;       // reset the counter to 0 so the counting starts over...
      currentState = STATE_IDLE;
      delay(10000);// put delay here
    }   
  }
}

