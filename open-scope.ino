#define dirPin 2
#define stepPin 3
#define rots 3
#define scale 200

bool dir = 0;

void setup() {
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

}

void loop() {
  digitalWrite(dirPin, dir);
  dir = !dir;
  delay(500);
  for (int i = 0; i < scale * rots; i++) {
    digitalWrite(stepPin, HIGH);
    delay(10);
    digitalWrite(stepPin, LOW);
    delay(10);
  }
}
