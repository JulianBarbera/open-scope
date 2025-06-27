// -------------------- Constants --------------------
#define dirPin 2
#define stepPin 3
#define scale 200
#define SUCCESS 0
#define FAILURE 1
#define step_per_rot 400
#define deg_per_step 1 / step_per_rot * 360

// -------------------- Function Declarations --------------------
bool raw_move(char axis, float degrees, float time);

// -------------------- Arduino Setup --------------------
void setup() {
  Serial.begin(115200);
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  while (!Serial) {} // Wait for serial to begin
  Serial.println("\nWelcome to Open Scope");
}

// -------------------- Arduino Loop --------------------
void loop() {}

// -------------------- Move an Axis --------------------
bool raw_move(char axis, float degrees, float time) {
  digitalWrite(dirPin, degrees > 0 ? true : false);
  float interval = (degrees * deg_per_step) / (2 * time);
  for (int i = 0; i < abs(degrees) * deg_per_step; i++) {
    digitalWrite(stepPin, HIGH);
    delay(interval);
    digitalWrite(stepPin, LOW);
    delay(interval);
  }
  return SUCCESS;
}