/*
ESP32-S2 LED pattern demo - 4 LEDs on GPIO 1,2,3,4 (active HIGH)
4 patterns x 4 s each, repeating: chase, binary counter, blink, breathe
Boot counter is shown on the LEDs in binary, so a crash loop is visible.
 */
#include <Arduino.h>   // esp_system.h (esp_reset_reason) is already included by the core

#define LED_COUNT     4
#define PATTERN_COUNT 4
#define PATTERN_MS    4000

const uint8_t LED_PINS[LED_COUNT] = {1, 2, 3, 4};

// Survive software resets / panics so we can report what happened last time
RTC_DATA_ATTR uint32_t bootCount = 0;
RTC_DATA_ATTR int lastPattern = -1;

static void setLeds(uint8_t mask) {
  for (int i = 0; i < LED_COUNT; i++) digitalWrite(LED_PINS[i], (mask >> i) & 1);
}

static uint8_t readLeds() {
  uint8_t m = 0;
  for (int i = 0; i < LED_COUNT; i++) if (digitalRead(LED_PINS[i])) m |= (1 << i);
  return m;
}

static const char* resetReasonStr(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_POWERON:  return "POWERON";
    case ESP_RST_SW:       return "SOFTWARE";
    case ESP_RST_PANIC:    return "PANIC (crash)";
    case ESP_RST_INT_WDT:  return "INTERRUPT WATCHDOG";
    case ESP_RST_TASK_WDT: return "TASK WATCHDOG";
    case ESP_RST_WDT:      return "OTHER WATCHDOG";
    case ESP_RST_BROWNOUT: return "BROWNOUT";
    default:               return "OTHER";
  }
}

typedef void (*PatternFn)(void);

struct Pattern {
  const char* name;
  PatternFn   run;
};   // FIX: missing semicolon

void patternChase();
void patternBinary();
void patternBlink();
void patternBreathe();

// FIX: all 4 entries now present (4th was NULL -> crash), no lambda needed
const Pattern patterns[PATTERN_COUNT] = {
  {"chase",   patternChase},
  {"binary",  patternBinary},
  {"blink",   patternBlink},
  {"breathe", patternBreathe},
};

void patternChase() {
  uint32_t start = millis();
  int pos = 0, dir = 1;
  while (millis() - start < PATTERN_MS) {
    setLeds(1 << pos);
    delay(120);
    pos += dir;
    if (pos == LED_COUNT - 1 || pos == 0) dir = -dir;
  }
}

void patternBinary() {
  uint32_t start = millis();
  uint8_t value = 0;
  while (millis() - start < PATTERN_MS) {
    for (int i = 0; i < LED_COUNT; i++) {
      digitalWrite(LED_PINS[i], (value >> i) & 1);   // FIX: operator precedence / wrong shift
    }
    delay(200);
    uint8_t shown = readLeds();

    // FIX: small stack buffer instead of leaked 8 KB malloc (also unchecked for NULL)
    char line[96];
    snprintf(line, sizeof(line), "binary: value=%2u expected=0x%X actual=0x%X %s",
             value, value & 0x0F, shown,
             (shown == (value & 0x0F)) ? "OK" : "MISMATCH");
    Serial.println(line);

    value = (value + 1) & 0x0F;
  }
}

void patternBlink() {   // FIX: matches the declaration (no parameter)
  uint32_t start = millis();
  while (millis() - start < PATTERN_MS) {
    setLeds(0x0F); delay(300);
    setLeds(0x00); delay(300);
  }
}

// Software PWM "breathing" on all LEDs (no dependency on LEDC API version)
void patternBreathe() {
  const uint32_t PERIOD_US = 2000;   // 500 Hz PWM
  const int STEPS = 100;             // ramp steps up and down
  uint32_t start = millis();
  int level = 0, dir = 1;
  while (millis() - start < PATTERN_MS) {
    uint32_t onUs  = (uint32_t)level * level * PERIOD_US / (STEPS * STEPS);  // gamma-ish curve
    uint32_t offUs = PERIOD_US - onUs;
    for (int rep = 0; rep < 2; rep++) {   // ~4 ms per step -> ~0.8 s full breath
      if (onUs)  { setLeds(0x0F); delayMicroseconds(onUs); }
      if (offUs) { setLeds(0x00); delayMicroseconds(offUs); }
    }
    level += dir;
    if (level >= STEPS) { level = STEPS; dir = -1; }
    if (level <= 0)     { level = 0;     dir = 1;  }
    if ((level & 0x0F) == 0) delay(1);   // yield to the scheduler/watchdog
  }
}

void ledTask(void* arg) {
  // FIX: was a 2400-byte buffer on a 2048-byte stack -> stack overflow
  Serial.println("[ledTask] started, patterns:");
  for (int i = 0; i < PATTERN_COUNT; i++) {
    Serial.printf("  [%d] %s\n", i, patterns[i].name);
  }

  for (;;) {
    for (int i = 0; i < PATTERN_COUNT; i++) {
      lastPattern = i;
      Serial.printf("[ledTask] running pattern %d (%s)\n", i, patterns[i].name);
      patterns[i].run();
      setLeds(0);
    }
  }
}

void setup() {
  Serial.begin(115200);
  uint32_t t0 = millis();
  while (!Serial && millis() - t0 < 3000) delay(10);

  for (int i = 0; i < LED_COUNT; i++) pinMode(LED_PINS[i], OUTPUT);

  bootCount++;
  Serial.println();
  Serial.printf("=== BOOT #%lu  reset reason: %s ===\n",
                (unsigned long)bootCount, resetReasonStr(esp_reset_reason()));
  Serial.printf("last pattern before reset: %d\n", lastPattern);

  // show boot count in binary on the LEDs
  setLeds(bootCount & 0x0F);
  delay(800);
  setLeds(0);

  // FIX: larger stack (4096) for printf/snprintf use
  xTaskCreate(ledTask, "ledTask", 4096, NULL, 1, NULL);
}

void loop() {
  Serial.printf("[heartbeat] up=%lus pattern=%d free_heap=%u min_free=%u\n",
                (unsigned long)(millis() / 1000), lastPattern,
                (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMinFreeHeap());
  delay(1000);
}
