// open-scope.cpp
#include "open-scope.h"

float multiplier = 1;

// Setup
void
setup()
{
	Serial.begin(9600);
	pinMode(kStepPin, OUTPUT);
	pinMode(kDirPin, OUTPUT);
	while (!Serial) {
	}

	Serial.println("\nWelcome to Open Scope\n");
	InitBno();
}

// Loop
void
loop()
{
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
}

// Command Handling
bool
HandleCommand(const String& input)
{
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
		CalibrateX();
	} else if (input.startsWith("heading")) {
		float heading = BnoHeadingDeg();
		Serial.println(heading);
		Serial.println();

		return true;
	}
	return false;
}

// Help Menu
void
PrintHelpMenu()
{
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
		"    heading           - print current heading with respect to magnetic north");
	Serial.println("                       e.g. heading\n");
}

// Move Command
bool
RawMove(char axis, float degrees, float time_sec)
{
	(void)axis; // axis currently unused

	digitalWrite(kDirPin, degrees > 0 ? LOW : HIGH);

	int step_count = round(abs(degrees) * kStepPerDeg * multiplier);
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
