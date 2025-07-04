// main.cpp
#include "bno.h"
#include <Adafruit_BNO055.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

#define kDirPin 2
#define kStepPin 3

#define uimu
#define ugps

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

const int kStepPerRot = 800;
const float kStepPerDeg = kStepPerRot / 360.0;

Adafruit_BNO055 bno = Adafruit_BNO055();

// Function Declarations
bool HandleCommand(const String &input);
bool RawMove(char axis, float degrees, float time_sec);
void PrintHelpMenu();
bool UpdateGPS();
void DisplayGPS();

// Variables
u32_t sats = 0;
double lat_d = 0;
double lng_d = 0;
double alt_m = 0;
double date_dmy = 0;
u32_t time_cs = 0;

// Setup
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600, SERIAL_8N1, 20, 21);
  pinMode(kStepPin, OUTPUT);
  pinMode(kDirPin, OUTPUT);
  while (!Serial) {
  }

  Serial.println("\nWelcome to Open Scope\n");
  InitBno(bno);
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

  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }
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
  } else if (input.startsWith("gps-print")) {
    DisplayGPS();
    return false;
  }

  return false;
}

// Help Menu
void PrintHelpMenu() {
  Serial.println();
  Serial.println("Welcome to Open Scope!");
  Serial.println("Available commands:");
  Serial.println("    help              - Show this menu");
  Serial.println("    raw <a> <d> <t>   - Move axis <a> by <d>° in <t> sec");
  Serial.println("                       e.g. raw X 90 2.0");
  Serial.println("    gps-print         - Print out all GPS data");
  Serial.println();
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

bool UpdateGPS() {
  if (gps.location.isValid()) {
    sats = gps.satellites.value();
    lat_d = gps.location.lat();
    lng_d = gps.location.lng();
    alt_m = gps.altitude.meters();
    date_dmy = gps.date.value();
    time_cs = gps.time.value();
    return true;
  }
  return false;
}

void DisplayGPS() {
  if (UpdateGPS()) {
    Serial.print("num sats: ");
    Serial.print(sats);
    Serial.print(" lat: ");
    Serial.print(lat_d);
    Serial.print(" lng: ");
    Serial.print(lng_d);
    Serial.print(" alt: ");
    Serial.print(alt_m);
    Serial.print(" date: ");
    Serial.print(date_dmy);
    Serial.print(" time: ");
    Serial.print(time_cs);
    Serial.println();
  } else {
    Serial.println("Satellite error.");
  }
}
