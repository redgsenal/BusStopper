#ifndef GPSCoor_h
#define GPSCoor_h
#include "Arduino.h"

class GPSCoor{
	public:
		GPSCoor(float lat, float lon);
		float lat();
		float lon();
		String coordinates();
	private:
		float _lat;
		float _lon;		
};

#endif;