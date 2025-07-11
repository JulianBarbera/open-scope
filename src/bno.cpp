// bno.cpp
#include "open-scope.h"
#include <math.h>
Adafruit_BNO055 bno = Adafruit_BNO055();

bool InitBno()
{

	bool is_connected = bno.begin();
	if (!is_connected)
		Serial.println("Can't connect to Adafruit BNO055!");
	/* Use external crystal for better accuracy (taken from BNO055 examples)
	 */
	bno.setExtCrystalUse(true);
	return is_connected;
}

bool IsValidAxis(char axis)
{
	return (axis == 'x' || axis == 'X' ||
            axis == 'y' || axis == 'Y' ||
            axis == 'z' || axis == 'Z');
}

float BnoAxisDeg(char axis)
{
	/* Event API found at
	 * https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h
	 */
	sensors_event_t event;
	bno.getEvent(&event);
	switch (axis) {
	case 'y':
		return event.orientation.y;
	case 'z':
		return event.orientation.z;
	case 'x':
	default:
		return event.orientation.x;
	}
}

float BnoHeadingDeg()
{
    sensors_event_t magneticVec;
    bno.getEvent(&magneticVec, Adafruit_BNO055::VECTOR_MAGNETOMETER);

    // update to account for tilt?
    float tan_theta = magneticVec.magnetic.y / magneticVec.magnetic.x;
    float theta = atan(tan_theta);
    float theta_deg = theta * SENSORS_RADS_TO_DPS;
    if (theta_deg < 0)
        theta_deg += 360;

    return theta_deg;
}

void Calibrate()
{
	while (BnoAxisDeg('x')) {
	}
}
