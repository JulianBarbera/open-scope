// bno.cpp
#include <math.h>

#include "open-scope.h"

Adafruit_BNO055 bno = Adafruit_BNO055();

bool InitBno() {
  bool is_connected = bno.begin();
  if (!is_connected) Serial.println("Can't connect to Adafruit BNO055!");
  /* Use external crystal for better accuracy (taken from BNO055 examples)
   */
  bno.setExtCrystalUse(true);
  return is_connected;
}

bool IsValidAxis(char axis) {
  return (axis == 'x' || axis == 'X' || axis == 'y' || axis == 'Y' ||
          axis == 'z' || axis == 'Z');
}

float BnoAxisDeg(char axis) {
  /* Event API found at
   * https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h
   */
  sensors_event_t event;
  bno.getEvent(&event);
  switch (axis) {
    case 'y':
    case 'Y':
      return event.orientation.y;
    case 'z':
    case 'Z':
      return event.orientation.z;
    case 'x':
    case 'X':
    default:
      return event.orientation.x;
  }
}

float BnoHeadingDeg() {
  sensors_event_t magneticVec;
  bno.getEvent(&magneticVec, Adafruit_BNO055::VECTOR_MAGNETOMETER);

  // update to account for tilt?
  float tan_theta = magneticVec.magnetic.y / magneticVec.magnetic.x;
  float theta = atan(tan_theta);
  float theta_deg = theta * SENSORS_RADS_TO_DPS;
  if (theta_deg < 0) theta_deg += 360;

  return theta_deg;
}

void CalibrateX(float &multiplier) {
  Serial.println("Begining calibration sequence");
  int calibrationStep = 1;      // Degrees
  float calibrationTime = 0.5;  // Time to execute step
  float tolerance = 5;          // Tolerance of calibration
  int degOut = 0;
  if (abs(BnoAxisDeg('x')) >= tolerance) {
    Serial.println("Please point scope perpendicular to ground and try again.");
    return;
  }
  while (abs(90 - BnoAxisDeg('x')) >= tolerance) {
    RawMove('x', calibrationStep, calibrationTime, multiplier);
    degOut += calibrationStep;
    delay(calibrationTime);
    Serial.println(BnoAxisDeg('x'));
  }
  Serial.println("Calibration complete!");
  Serial.println("Results:");
  Serial.print("Degrees out: ");
  Serial.println(degOut);
  multiplier = 90 / (float)degOut;
  Serial.print("Multiplier: ");
  Serial.println(multiplier);
}
