const int ledPins[] = {1, 2, 3, 4}; // 4 consecutive GPIO pins (GPIO 1, 2, 3, 4)
const int numLeds = 4;
int delayTime = 90; // Speed of the Knight Rider scanner

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Scan forward: LED 1 -> 2 -> 3 -> 4
 /* for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }

  // Scan backward: LED 3 -> 2
  for (int i = numLeds - 2; i > 0; i--) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }*/
}
