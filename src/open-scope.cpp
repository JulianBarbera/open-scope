// main.cpp
#include "bno.h"
#include <Adafruit_BNO055.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

#define kDirPin 2
#define kStepPin 3

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

const int kStepPerRot = 800;
const float kStepPerDeg = kStepPerRot / 360.0;

Adafruit_BNO055 bno = Adafruit_BNO055();

// Function Declarations
bool HandleCommand(const String &input);
bool RawMove(char axis, float degrees, float time_sec);
void PrintHelpMenu();
void displayInfo();

// Setup
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600, SERIAL_8N1, 20, 21);
  pinMode(kStepPin, OUTPUT);
  pinMode(kDirPin, OUTPUT);
  while (!Serial) {
  }

  Serial.println("\nWelcome to Open Scope\n");
  // InitBno(bno);
}

// Loop
void loop() {
  if (Serial.available()) {
    String input_buffer = Serial.readStringUntil('\n');
    input_buffer.trim();

    if (input_buffer.equalsIgnoreCase("help")) {
      PrintHelpMenu();
    } else if (!HandleCommand(input_buffer)) {
      Serial.println(
          "Unknown command. Use \"help\" to list available commands.\n");
    }
  }

  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();
}

// Command Handling
bool HandleCommand(const String &input) {
  if (input.startsWith("raw")) {
    char axis;
    float degrees;
    float time_sec;

    int parsed =
        sscanf(input.c_str(), "raw %c %f %f", &axis, &degrees, &time_sec);
    if (parsed != 3) {
      Serial.println("Usage: raw <axis> <degrees> <time_sec>\n");
      return false;
    }
    return RawMove(axis, degrees, time_sec);
  } else if (input.startsWith("degrees")) {
    char axis;
    int parsed = sscanf(input.c_str(), "degrees %c", &axis);
    if (parsed != 1) {
      Serial.println("Usage: degrees <axis>\n");
      return false;
    }
    if (!IsValidAxis(axis)) {
      Serial.println("Error: Invalid axis specified.\n");
      return true;
    }

    Serial.println(BnoAxisDeg(bno, axis));
    Serial.println();
    return true;
  }

  return false;
}

// Help Menu
void PrintHelpMenu() {
  Serial.println("\nWelcome to Open Scope!");
  Serial.println("Available commands:");
  Serial.println("    help              - Show this menu");
  Serial.println("    raw <a> <d> <t>   - Move axis <a> by <d>° in <t> sec");
  Serial.println("                       e.g. raw X 90 2.0\n");
}

// Move Command
bool RawMove(char axis, float degrees, float time_sec) {
  (void)axis; // axis currently unused

  digitalWrite(kDirPin, degrees > 0 ? LOW : HIGH);

  int step_count = round(abs(degrees) * kStepPerDeg);
  if (step_count == 0 || time_sec <= 0) {
    Serial.println("Invalid parameters for movement.");
    return false;
  }

  float interval_ms = (time_sec * 1000) / (2 * step_count);

  Serial.println("Performing raw move.");
  Serial.print("Steps: ");
  Serial.println(step_count);
  Serial.print("Interval: ");
  Serial.print(interval_ms);
  Serial.println(" ms\n");

  for (int i = 0; i < step_count; ++i) {
    digitalWrite(kStepPin, HIGH);
    delay(interval_ms);
    digitalWrite(kStepPin, LOW);
    delay(interval_ms);
  }

  Serial.println("Raw move complete.\n");
  return true;
}
void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
