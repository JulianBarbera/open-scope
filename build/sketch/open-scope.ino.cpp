#include <Arduino.h>
#line 1 "/home/fish/devel/open-scope/open-scope.ino"
#define dirPin 2
#define stepPin 3
#define rots 4
#define scale 200

bool dir = 0;

#line 8 "/home/fish/devel/open-scope/open-scope.ino"
void setup();
#line 15 "/home/fish/devel/open-scope/open-scope.ino"
void loop();
#line 8 "/home/fish/devel/open-scope/open-scope.ino"
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
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
  }
}

