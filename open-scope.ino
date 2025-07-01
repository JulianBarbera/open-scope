// -------------------- Constants --------------------
#include <Adafruit_BNO055.h>
#include "bno.h"
#define kDirPin 2
#define kStepPin 3

const int kStepPerRot = 800;
const float kStepPerDeg = kStepPerRot / 360.0;

// -------------------- Function Declarations --------------------
bool HandleCommand(const String& input);
bool RawMove(char axis, float degrees, float time_sec);
void PrintHelpMenu();

Adafruit_BNO055 bno = Adafruit_BNO055();
// -------------------- Arduino Setup --------------------
void setup() {
  Serial.begin(9600);
  pinMode(kStepPin, OUTPUT);
  pinMode(kDirPin, OUTPUT);
  while (!Serial) {}  // Wait for serial to be ready
  Serial.println("\nWelcome to Open Scope");
  Serial.println();
  InitBno(bno);
}

// -------------------- Arduino Loop --------------------
void loop() {
  if (Serial.available()) {
    String input_buffer = Serial.readStringUntil('\n');
    input_buffer.trim();

    if (input_buffer.equalsIgnoreCase("help")) {
      PrintHelpMenu();
    } else if (!HandleCommand(input_buffer)) {
      Serial.println(
          "Unknown command. Use \"help\" to list available commands.");
      Serial.println();
    }
  }
}

// -------------------- Command Handlers --------------------
bool HandleCommand(const String& input) {
  if (input.startsWith("raw")) {
    char axis;
    float degrees;
    float time_sec;

    int parsed = sscanf(input.c_str(), "raw %c %f %f",
                        &axis, &degrees, &time_sec);
    if (parsed != 3) {
      Serial.println("Usage: raw <axis> <degrees> <time_sec>");
      Serial.println();
      return false;
    }

    return RawMove(axis, degrees, time_sec);
  }
  else if (input.startsWith("degrees"))
  {
    char axis;
    int parsed = sscanf(input.c_str(), "degrees %c", &axis);
    if (parsed != 1)
    {
      Serial.println("Usage: degrees <axis>");
      Serial.println();
      return false;
    }
    if (!IsValidAxis(axis))
    {
      Serial.println("Error: Invalid axis specified.");
      Serial.println();
      return true;
    }
    Serial.println(BnoAxisDeg(bno, axis));
    Serial.println();
    return true;
  }

  return false;
}

// -------------------- Help Menu --------------------
void PrintHelpMenu() {
  Serial.println("\nWelcome to Open Scope!");
  Serial.println("Available commands:");
  Serial.println("    help              - Show this menu");
  Serial.println("    raw <a> <d> <t>   - Move axis <a> by <d>° in <t> sec");
  Serial.println("                       e.g. raw X 90 2.0");
  Serial.println();
}

// -------------------- Move an Axis --------------------
bool RawMove(char axis, float degrees, float time_sec) {
  // axis parameter currently unused
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
  Serial.print(" ms\n");


  for (int i = 0; i < step_count; ++i) {
    digitalWrite(kStepPin, HIGH);
    delay(interval_ms);
    digitalWrite(kStepPin, LOW);
    delay(interval_ms);
  }
  Serial.println("Raw move complete.");
  Serial.println();
  return true;
}
