// ================= PIN CONFIG =================
#define TRIG_PIN 25
#define ECHO_PIN 27

#define BUZZER_PIN 33
#define MOTOR_PIN 26
#define LED_GREEN 14
#define LED_RED 12

#define BUTTON_PIN 32
#define BATT_ADC 34

// ================= CONSTANT =================
#define MAX_DISTANCE 400
#define TIMEOUT 30000
#define SAMPLE_COUNT 5
#define BLIND_ZONE 20
#define LOW_BATTERY 3.5

unsigned long previousMillis = 0;
bool redState = false;

// ================= SETUP =================
void setup() {

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_GREEN, HIGH);

  Serial.begin(115200);
}

// ================= BATTERY READ =================
float readBattery() {
  int adc = analogRead(BATT_ADC);
  float voltage = (adc / 4095.0) * 3.3;
  voltage *= 2.0;  // divider 100k-100k
  return voltage;
}

// ================= ULTRASONIC =================
float readUltrasonic() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, TIMEOUT);

  if (duration == 0) return MAX_DISTANCE;

  return duration * 0.0343 / 2.0;
}

float getMedianDistance() {

  float temp[SAMPLE_COUNT];

  for (int i = 0; i < SAMPLE_COUNT; i++) {
    temp[i] = readUltrasonic();
    delay(60);
  }

  for (int i = 0; i < SAMPLE_COUNT - 1; i++) {
    for (int j = i + 1; j < SAMPLE_COUNT; j++) {
      if (temp[j] < temp[i]) {
        float swap = temp[i];
        temp[i] = temp[j];
        temp[j] = swap;
      }
    }
  }

  float result = temp[SAMPLE_COUNT / 2];

  if (result < BLIND_ZONE) result = BLIND_ZONE;
  if (result > MAX_DISTANCE) result = MAX_DISTANCE;

  return result;
}

// ================= LOOP =================
void loop() {

  unsigned long currentMillis = millis();

  // ===== PRIORITAS 1: EMERGENCY =====
  if (digitalRead(BUTTON_PIN) == LOW) {

    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(MOTOR_PIN, HIGH);

    if (currentMillis - previousMillis >= 300) {
      previousMillis = currentMillis;
      redState = !redState;
      digitalWrite(LED_RED, redState);
    }

    return;
  }

  // ===== NORMAL MODE =====
  float distance = getMedianDistance();
  float battVoltage = readBattery();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Battery: ");
  Serial.println(battVoltage);

  // ===== BATTERY INDICATOR =====
  if (battVoltage < LOW_BATTERY) {
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_RED, LOW);
  }

  // ===== OBSTACLE LOGIC =====
  if (distance < 100) {

    int interval;

    if (distance < 30) interval = 100;
    else if (distance < 60) interval = 250;
    else interval = 500;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(MOTOR_PIN, HIGH);
      delay(50);
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(MOTOR_PIN, LOW);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  }

  delay(50);
}
