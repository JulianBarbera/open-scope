// open-scope.cpp
#include "open-scope.h"

// Setup
void setup() {
  Serial.begin(9600);
  pinMode(kStepPin, OUTPUT);
  pinMode(kDirPin, OUTPUT);
  while (!Serial) {}
  while (!Serial.available()) {}
  InitBno();
  Serial.print("\nWelcome to Open Scope\n> ");
}

enum TtyState {
  normal,
  arrow_1,
  arrow_2,
};

// Loop
void loop() {
  // handy shell with backspace + arrow key + history support
  static unsigned int cursor = 0;
  static String input_buffer = "";
  static String history = "";
  static int history_cursor = 0;
  static TtyState state = normal;
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      Serial.write("\n");
      input_buffer.trim();
      if (input_buffer.length() > 0) {
        if (history.length() > 0)
          history += "\n";
        history += input_buffer;
        history_cursor = history.length();

        if (input_buffer.equalsIgnoreCase("help")) {
          PrintHelpMenu();
        } else if (!HandleCommand(input_buffer)) {
          Serial.println(
              "Unknown command. Use \"help\" to list available commands.\n");
        }
      }
      input_buffer = "";
      state = normal;
      cursor = 0;
      Serial.write("> ");
    } else if (c == 0x8 /* backspace */) {
      if (cursor > 0) {
        if (cursor == input_buffer.length()) {
          Serial.write("\x1b[1D \x1b[1D");
        } else {
          Serial.write("\x1b[1D");
          String after = input_buffer.substring(cursor);
          Serial.print(after);
          Serial.write(" \x1b[");
          Serial.print(after.length() + 1, 10);
          Serial.write("D");
        }
        input_buffer = input_buffer.substring(0, cursor - 1) +
                       input_buffer.substring(cursor);
        cursor -= 1;
      }
    } else if (c == 12 /* Ctrl+L */) {
      Serial.write("\x1b[2J\x1b[H> ");
      Serial.print(input_buffer);
    } else if (c == 27) {
      state = arrow_1;
    } else if (state == arrow_1) {
      if (c == '[') {
        state = arrow_2;
      } else {
        state = normal;
      }
    } else if (state == arrow_2) {
      state = normal;
      if (c == 'C' /* Arrow Right */) {
        unsigned int len = input_buffer.length();
        if (cursor < len) {
          cursor += 1;
          Serial.write("\x1b[1C");
        }
      } else if (c == 'D' /* Arrow Left */) {
        if (cursor > 0) {
          cursor -= 1;
          Serial.write("\x1b[1D");
        }
      } else if (c == 'A' /* Arrow Up */) {
        if (history_cursor == -1)
          return;
        int start = history.lastIndexOf('\n', history_cursor - 1);
        if (start == -1)
          start = -1;
        String line = history.substring(start + 1, history_cursor);
        input_buffer = line;
        Serial.write("\x1b[1K\r> ");
        Serial.print(line);
        cursor = input_buffer.length();
        history_cursor = start;
      } else if (c == 'B' /* Arrow Down */) {
        if (history_cursor >= (int)(history.length()))
          return;
        int start = history.indexOf('\n', history_cursor + 1);
        if (start == -1)
          start = history.length();
        else
          start += 1;
        int end = history.indexOf('\n', start);
        if (end == -1)
          end = history.length();
        String line = history.substring(start, end);
        input_buffer = line;
        Serial.write("\x1b[1K\r> ");
        Serial.print(line);
        cursor = input_buffer.length();
        history_cursor = end;
      }
    } else if (c != '\r') {
      if ((c == ' ') || (c > 32 && c < 127)) {
        unsigned int len = input_buffer.length();
        if (cursor == len) {
          // append
          input_buffer += c;
          Serial.write(c);
        } else {
          // insert
          String before = input_buffer.substring(0, cursor);
          String after = input_buffer.substring(cursor);
          input_buffer = before + String(c) + after;
          Serial.write(c);
          Serial.print(after);
          Serial.write("\x1b[");
          Serial.print(after.length(), 10);
          Serial.write("D");
        }
        cursor += 1;
      }
    }
  }
}

// Command Handling
bool HandleCommand(const String& input) {
  static float multiplier = 9;

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

    return RawMove(axis, degrees, time_sec, multiplier);
  } else if (input.startsWith("degrees")) {
    char axis;
    int parsed = sscanf(input.c_str(), "degrees %c", &axis);
    if (parsed != 1) {
      Serial.println("Usage: degrees <a>\n");
      return false;
    }
    if (!IsValidAxis(axis)) {
      Serial.println("Error: Invalid axis specified.\n");
      return true;
    }

    Serial.println(BnoAxisDeg(axis));
    Serial.println();
    return true;
  } else if (input.startsWith("calibrate")) {
    CalibrateX(multiplier);
  } else if (input.startsWith("heading")) {
    float heading = BnoHeadingDeg();
    Serial.println(heading);
    Serial.println();

    return true;
  } else if (input.startsWith("snow")) {
    Serial.println("\x1b[36msnow\x1b[0m");
    return true;
  }
  return false;
}

// Help Menu
void PrintHelpMenu() {
  Serial.println("\nWelcome to Open Scope!");
  Serial.println("Available commands:");
  Serial.println("    help              - Show this menu\n");

  Serial.println("    raw <a> <d> <t>   - Move axis <a> by <d>° in <t> sec");
  Serial.println("                       e.g. raw X 90 2.0\n");

  Serial.println(
      "    degrees <a>       - display orientation of axis <a> in degrees");
  Serial.println("                      - takes in axes 'x', 'y', or 'z'\n");
  Serial.println("    calibrate         - Calibrate X axis stepper motor");
  Serial.println("                       e.g. calibrate\n");

  Serial.println(
      "    heading           - print current heading with respect to "
      "magnetic "
      "north");
  Serial.println("                       e.g. heading\n");
}

// Move Command
bool RawMove(char axis, float degrees, float time_sec, float multiplier = 1) {
  (void)axis;  // axis currently unused

  digitalWrite(kDirPin, degrees > 0 ? LOW : HIGH);

  int step_count = round(abs(degrees) * kStepPerDeg * multiplier);
  float interval_us = (time_sec * 1000 * 1000) / (2 * step_count);
  interval_us = interval_us < 150 ? 150 : interval_us;
  if (step_count == 0 || time_sec <= 0) {
    Serial.println("Invalid parameters for movement.");
    return false;
  }

  Serial.println("Performing raw move.");
  Serial.print("Steps: ");
  Serial.println(step_count);
  Serial.print("Interval: ");
  Serial.print(interval_us);
  Serial.println(" us\n");

  for (int i = 0; i < step_count; ++i) {
    digitalWrite(kStepPin, HIGH);
    delayMicroseconds(interval_us);
    digitalWrite(kStepPin, LOW);
    delayMicroseconds(interval_us);
  }

  Serial.println("Raw move complete.\n");
  return true;
}
