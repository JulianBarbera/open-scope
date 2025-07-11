// bno.cpp
#include "open-scope.h"
Adafruit_BNO055 bno = Adafruit_BNO055();

bool
InitBno()
{

	bool is_connected = bno.begin();
	if (!is_connected)
		Serial.println("Can't connect to Adafruit BNO055!");
	/* Use external crystal for better accuracy (taken from BNO055 examples)
	 */
	bno.setExtCrystalUse(true);
	return is_connected;
}

bool
IsValidAxis(char axis)
{
	if (axis >= 'A' && axis <= 'Z')
		axis += 32;
	return (axis == 'x' || axis == 'y' || axis == 'z');
}
float
BnoAxisDeg(char axis)
{
	/* Event API found at
	 * https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h
	 */
	sensors_event_t event;
	bno.getEvent(&event);
	switch (axis) {
	case 'x':
		return event.orientation.x;
	case 'y':
		return event.orientation.y;
	case 'z':
		return event.orientation.z;
	default:
		return event.orientation.x;
	}
}

void
Calibrate()
{
	while (BnoAxisDeg('x')) {
	}
}
