#include <Adafruit_BNO055.h>
bool InitBno(Adafruit_BNO055 &bno)
{
  bool is_connected = bno.begin();
  if (!is_connected)
    Serial.println("Can't connect to Adafruit BNO055!");

  /* Use external crystal for better accuracy (taken from BNO055 examples) */
  bno.setExtCrystalUse(true);
  return is_connected;
}

inline bool IsValidAxis(char axis)
{
  if (axis >= 'A' && axis <= 'Z')
    axis += 32;
  return (axis == 'x' || axis == 'y' || axis == 'z');
}
float BnoAxisDeg(Adafruit_BNO055 &bno, char axis)
{
/* Event API found at https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h */
  sensors_event_t event;
  bno.getEvent(&event);
  switch (axis)
  {
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

void Calibrate(Adafruit_BNO055 &bno)
{

}
