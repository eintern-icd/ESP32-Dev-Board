// VIP LED Pattern with Initial Double Blinks for GPIO 1, 2, 3, and 4

const int leds[] = {1, 2, 3, 4};
const int numLeds = 4;

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void clearLeds() {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], LOW);
  }
}

void setAllLeds(bool state) {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(leds[i], state ? HIGH : LOW);
  }
}

// Flash GPIO 1 & 2 twice
void blinkGpio1and2Twice() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(leds[0], HIGH);
    digitalWrite(leds[1], HIGH);
    digitalWrite(leds[2], LOW);
    digitalWrite(leds[3], LOW);
    delay(150);
    
    digitalWrite(leds[0], LOW);
    digitalWrite(leds[1], LOW);
    delay(150);
  }
}

// Flash GPIO 3 & 4 twice
void blinkGpio3and4Twice() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(leds[0], LOW);
    digitalWrite(leds[1], LOW);
    digitalWrite(leds[2], HIGH);
    digitalWrite(leds[3], HIGH);
    delay(150);
    
    digitalWrite(leds[2], LOW);
    digitalWrite(leds[3], LOW);
    delay(150);
  }
}

// VIP Pattern 1: Fast Ping-Pong Sweep
void pingPongSweep(int speedMs, int cycles) {
  for (int c = 0; c < cycles; c++) {
    for (int i = 0; i < numLeds; i++) {
      clearLeds();
      digitalWrite(leds[i], HIGH);
      delay(speedMs);
    }
    for (int i = numLeds - 2; i > 0; i--) {
      clearLeds();
      digitalWrite(leds[i], HIGH);
      delay(speedMs);
    }
  }
}

// VIP Pattern 2: High-intensity Strobe
void vipStrobe(int flashes, int flashDelay) {
  for (int i = 0; i < flashes; i++) {
    setAllLeds(true);
    delay(flashDelay);
    setAllLeds(false);
    delay(flashDelay);
  }
}

// VIP Pattern 3: Alternating Pairs
void alternatingFlashes(int speedMs, int cycles) {
  for (int c = 0; c < cycles; c++) {
    digitalWrite(leds[0], HIGH);
    digitalWrite(leds[1], LOW);
    digitalWrite(leds[2], LOW);
    digitalWrite(leds[3], HIGH);
    delay(speedMs);

    digitalWrite(leds[0], LOW);
    digitalWrite(leds[1], HIGH);
    digitalWrite(leds[2], HIGH);
    digitalWrite(leds[3], LOW);
    delay(speedMs);
  }
}

void loop() {
  // 1. Blink GPIO 1 & 2 twice
  blinkGpio1and2Twice();
  delay(100);

  // 2. Blink GPIO 3 & 4 twice
  blinkGpio3and4Twice();
  delay(100);

  // 3. VIP Pattern sequence
  pingPongSweep(60, 4);
  alternatingFlashes(100, 6);
  vipStrobe(8, 40);

  clearLeds();
  delay(300); // Brief pause before repeating full loop
}
