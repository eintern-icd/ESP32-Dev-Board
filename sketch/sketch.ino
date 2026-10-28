// ESP32-S2 – 4 LED pattern blinker
// LEDs on GPIO 1, 2, 3, 4 (active HIGH: GPIO -> resistor -> LED -> GND)

const uint8_t ledPins[] = {1, 2, 3, 4};
const uint8_t numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

// Write a 4-bit mask to the LEDs (bit0 = GPIO1 ... bit3 = GPIO4)
void setLeds(uint8_t mask) {
  for (uint8_t i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], (mask >> i) & 0x01 ? HIGH : LOW);
  }
}

// Pattern 1: scanner (Knight Rider) back and forth
void patternScanner(uint8_t cycles, uint16_t stepMs) {
  for (uint8_t c = 0; c < cycles; c++) {
    for (int i = 0; i < numLeds; i++) {
      setLeds(1 << i);
      delay(stepMs);
    }
    for (int i = numLeds - 2; i > 0; i--) {
      setLeds(1 << i);
      delay(stepMs);
    }
  }
  setLeds(0);
}

// Pattern 2: all LEDs blink together
void patternAllBlink(uint8_t times, uint16_t onOffMs) {
  for (uint8_t t = 0; t < times; t++) {
    setLeds(0x0F);
    delay(onOffMs);
    setLeds(0x00);
    delay(onOffMs);
  }
}

// Pattern 3: alternate odd / even LEDs
void patternAlternate(uint8_t times, uint16_t stepMs) {
  for (uint8_t t = 0; t < times; t++) {
    setLeds(0b0101);
    delay(stepMs);
    setLeds(0b1010);
    delay(stepMs);
  }
  setLeds(0);
}

// Pattern 4: fill up one by one, then clear one by one
void patternFillUp(uint8_t cycles, uint16_t stepMs) {
  for (uint8_t c = 0; c < cycles; c++) {
    uint8_t mask = 0;
    for (int i = 0; i < numLeds; i++) {
      mask |= (1 << i);
      setLeds(mask);
      delay(stepMs);
    }
    for (int i = 0; i < numLeds; i++) {
      mask &= ~(1 << i);
      setLeds(mask);
      delay(stepMs);
    }
  }
}

// Pattern 5: binary counter 0..15
void patternBinaryCount(uint16_t stepMs) {
  for (uint8_t n = 0; n < 16; n++) {
    setLeds(n);
    delay(stepMs);
  }
  setLeds(0);
}

// Pattern 6: outside-in / inside-out
void patternCenterBurst(uint8_t cycles, uint16_t stepMs) {
  for (uint8_t c = 0; c < cycles; c++) {
    setLeds(0b0110);  // center two
    delay(stepMs);
    setLeds(0b1111);  // all
    delay(stepMs);
    setLeds(0b1001);  // outer two
    delay(stepMs);
    setLeds(0b0000);
    delay(stepMs);
  }
}

void setup() {
  Serial.begin(115200);
  for (uint8_t i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  Serial.println("ESP32-S2 LED pattern demo started (GPIO 1,2,3,4)");
}

void loop() {
  Serial.println("Pattern: scanner");
  patternScanner(3, 90);

  Serial.println("Pattern: all blink");
  patternAllBlink(4, 200);

  Serial.println("Pattern: alternate");
  patternAlternate(6, 150);

  Serial.println("Pattern: fill up");
  patternFillUp(3, 120);

  Serial.println("Pattern: binary count");
  patternBinaryCount(200);

  Serial.println("Pattern: center burst");
  patternCenterBurst(4, 150);
}
