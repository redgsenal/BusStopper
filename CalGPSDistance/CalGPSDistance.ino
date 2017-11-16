#include <GPSCoor.h>
#define GPS_STATION_DISTANCE_RANGE 10 // 10m radius from bus station
#define NUMBER_OF_STATIONS 4
#define d2r 0.01745329251994329576923690768489
// bus stations
GPSCoor busstops[NUMBER_OF_STATIONS] = {
  GPSCoor(1.2312312, 103.231232),
  GPSCoor(1.3212, 103.221232),
  GPSCoor(1.66323, 103.092321),
  GPSCoor(1.9834534, 103.90218312)  
};
void setup() {
  Serial.begin(9600);
  while (!Serial){};
  Serial.println("GPS Distance...");
  boolean v = is_near_station(GPSCoor(1.32232, 103.231232));
  Serial.println("v: ");Serial.print(v);
}
void loop() {
  // put your main code here, to run repeatedly:
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
