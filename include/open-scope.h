// open-scope.h
#ifndef OPEN_SCOPE_H
#define OPEN_SCOPE_H

#include <Adafruit_BNO055.h>
#include <Arduino.h>

// constants
#define kDirPin 2
#define kStepPin 3

const int kStepPerRot = 800;
const float kStepPerDeg = kStepPerRot / 360.0;

extern float multiplier;

extern Adafruit_BNO055 bno;

// Function Declarations
bool HandleCommand(const String &input);
bool RawMove(char axis, float degrees, float time_sec);
void PrintHelpMenu();

// BNO Function Decs
float BnoAxisDeg(char axis);
bool InitBno();
void CalibrateX();
bool IsValidAxis(char axis);

#endif
