// Test code for Ultimate GPS Using Hardware Serial (e.g. GPS Flora or FeatherWing)
//
// This code shows how to listen to the GPS module via polling. Best used with
// Feathers or Flora where you have hardware Serial and no interrupt
//
// Tested and works great with the Adafruit GPS FeatherWing
// ------> https://www.adafruit.com/products/3133
// or Flora GPS
// ------> https://www.adafruit.com/products/1059
// but also works with the shield, breakout
// ------> https://www.adafruit.com/products/1272
// ------> https://www.adafruit.com/products/746
// 
// Pick one up today at the Adafruit electronics shop
// and help support open source hardware & software! -ada
     
#include <Adafruit_GPS.h>
#include <Wire.h>
#include "DFRobot_RGBLCD.h"

#include <GPSCoor.h>
#define GPS_STATION_DISTANCE_RANGE 10 // 10m radius from bus station
#define NUMBER_OF_STATIONS 4
#define d2r 0.01745329251994329576923690768489
// bus stations
// GPSCoor(latitude, longitude)
GPSCoor busstops[NUMBER_OF_STATIONS] = {
  GPSCoor(1.2312312, 103.231232),
  GPSCoor(1.19212, 103.51232),
  GPSCoor(1.66323, 103.092321),
  GPSCoor(1.9834534, 103.90218312)  
};

// what's the name of the hardware serial port?
#define GPSSerial Serial1
DFRobot_RGBLCD lcd(16,2);  //16 characters and 2 lines of show

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
     
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

void setup()
{
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");
     
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
  lcd.init();
  lcd.setRGB(0, 0, 255); // blue lcd BG
  lcd.setCursor(0,0);
  lcd.print("GPS ready...");
  delay(3000);
}

void loop() // run over and over again
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();
     
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    /*Serial.print("\nTime: ");
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);*/
    lcd.clear();
    if (GPS.fix) {
      /*Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      */
      lcd.setCursor(0, 0);
      lcd.print(GPS.latitude / 100);
      lcd.setCursor(0, 1);
      lcd.print(GPS.longitude / 100);
      delay(1000);
      lcd.clear();
      boolean v = is_near_station(GPSCoor((GPS.latitude / 100), (GPS.longitude / 100)));
      Serial.println("v: ");Serial.print(v);  
      lcd.setCursor(0, 0);
      if (v)
        lcd.print("Bus stop");
      else
        lcd.print("In-transit");      
    }else{
      lcd.setCursor(0, 0);
      lcd.print("     No GPS     ");
    }
  }
}
// checks if gps distance with tolerable distance range from the list of bus stations
boolean is_near_station(GPSCoor coor){
  Serial.println("cal distance ");
  for(int i = 0; i < NUMBER_OF_STATIONS; ++i){
    Serial.println(i);
    if (gpsdistance(coor, busstops[i]) <= GPS_STATION_DISTANCE_RANGE){
      return true;
    }
  }
  return false;
}
float gpsdistance(GPSCoor o1, GPSCoor o2){
  Serial.print("checking...");
  Serial.print(o1.coordinates());Serial.print(" - ");
  Serial.println(o2.coordinates());
  float d = haversine_km(o1.lat(), o1.lon(), o2.lat(), o2.lon());
  Serial.print("distance: ");Serial.println(d);
  return d;
}
// calculate haversine distance in kilometers for linear distance; coordinates in degress
float haversine_km(float lat1, float long1, float lat2, float long2){
    float dlong = (long2 - long1) * d2r;
    float dlat = (lat2 - lat1) * d2r;
    float a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    float d = 6367 * c;
    return d;
}

