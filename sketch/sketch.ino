// Greenhouse data logger (simulated sensors, no hardware needed)

#define RGB_LED_PIN 18   // onboard RGB LED (ESP32-S2-DevKitM-1 uses GPIO 18)

// External LEDs on GPIO 1, 2, 3, 4
const int LED_PINS[4] = {1, 2, 3, 4};
const int NUM_LEDS = 4;

// Runs in the background so the main code and its delay() are not affected
void blinkTask(void *param) {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  while (true) {
    for (int i = 0; i < NUM_LEDS; i++) digitalWrite(LED_PINS[i], HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    for (int i = 0; i < NUM_LEDS; i++) digitalWrite(LED_PINS[i], LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

const int NUM_READINGS = 5;
int temps[NUM_READINGS] = {22, 25, 24, 27, 23};
const char *names[3] = {"Temperature", "Humidity", "Light"};
int round_no = 0;
unsigned long lastPumpTime = 0;

int countCalls() {
  static int calls = 0;
  calls++;
  return calls;
}

float averageTemp() {
  int sum = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    sum += temps[i];
  }
  return (float)sum / NUM_READINGS;
}

int maxTemp() {
  int best = temps[0];
  for (int i = 1; i < NUM_READINGS; i++) {
    if (temps[i] > best) {
      best = temps[i];
    }
  }
  return best;
}

float toFahrenheit(int c) {
  return c * (9.0f / 5.0f) + 32.0f;
}

int totalWater() {
  int total = 0;
  const int pulses[3] = {100, 120, 90};
  for (int i = 0; i < 3; i++) {
    total += pulses[i];
  }
  return total;
}

String getLabel(int id) {
  return String("Zone-") + id;
}

bool pumpDue() {
  unsigned long now = millis();
  if (now - lastPumpTime > 5000UL) {
    lastPumpTime = now;
    return true;
  }
  return false;
}

void checkAlarm(int level) {
  int state = 0;
  if (level > 80) {
    state = 2;
  }
  if (state == 2) {
    Serial.println("ALARM: level critical");
  }
}

void printMode(int mode) {
  switch (mode) {
    case 0:
      Serial.println("Mode: OFF");
      break;
    case 1:
      Serial.println("Mode: AUTO");
      break;
    case 2:
      Serial.println("Mode: MANUAL");
      break;
  }
}

void handleCommand() {
  const char *cmd = "ON";
  if (strcmp(cmd, "ON") == 0) {
    Serial.println("Pump started");
  } else {
    Serial.println("Unknown command");
  }
}

void listSensors() {
  for (int i = 0; i < 3; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(names[i]);
  }
}

void setup() {
  Serial.begin(115200);
  // USB CDC: wait up to 3 s for the host to open the port so no output is lost
  unsigned long t0 = millis();
  while (!Serial && millis() - t0 < 3000) {
    delay(10);
  }
  Serial.println("=== Greenhouse Logger ===");
  neopixelWrite(RGB_LED_PIN, 0, 20, 0);   // LED on (green)
  xTaskCreate(blinkTask, "blinkTask", 4096, NULL, 1, NULL);   // start LED blinking
}

void loop() {
  // After 3 rounds, keep printing a heartbeat so the output is visible
  // whenever the serial monitor connects.
  if (round_no >= 3) {
    Serial.print("Done. Running OK, uptime ");
    Serial.print(millis() / 1000);
    Serial.println(" s");
    delay(3000);
    return;
  }

  round_no++;
  Serial.print("--- Round ");
  Serial.print(round_no);
  Serial.println(" ---");

  Serial.print("Zone: ");
  Serial.println(getLabel(round_no));

  Serial.print("Call count: ");
  Serial.println(countCalls());

  Serial.print("Average temp: ");
  Serial.println(averageTemp());

  Serial.print("Max temp: ");
  Serial.println(maxTemp());

  Serial.print("25 C in F: ");
  Serial.println(toFahrenheit(25));

  Serial.print("Total water: ");
  Serial.println(totalWater());

  checkAlarm(40);
  printMode(0);
  handleCommand();
  listSensors();

  if (pumpDue()) {
    Serial.println("Pump cycle due");
  }

  delay(2000);
}
