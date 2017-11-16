#include "Arduino.h"
#include "GPSCoor.h"
#include <stdio.h>


GPSCoor::GPSCoor(float lat, float lon){
  _lat = lat;
  _lon = lon;
}

float GPSCoor::lat(){
	return _lat;
}

float GPSCoor::lon() {
	return _lon;
}
String GPSCoor::coordinates(){
	char t[50];
	char n[50];
	sprintf(t , "%f", _lat);
	sprintf(n, "%f", _lon);
	return String(t) + String(", ") + String(n);
}

