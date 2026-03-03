#define TRIG_PIN 25
#define ECHO_PIN 27

#define BUZZER_PIN 33
#define MOTOR_PIN 26
#define LED_GREEN 12
#define LED_RED 14

#define BUTTON_PIN 32
#define BATT_ADC 34

// ================= CONSTANT =================
#define MAX_DISTANCE 400
#define TIMEOUT 30000
#define SAMPLE_COUNT 5
#define BLIND_ZONE 20
#define LOW_BATTERY 3.3

// ================= GLOBAL =================
unsigned long now;
unsigned long lastTrigger = 0;
bool outputState = false;

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
  analogSetAttenuation(ADC_11db);
}

// ================= BATTERY =================
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

  now = millis();

  float distance = getMedianDistance();
  float battVoltage = readBattery();

  // ================= SERIAL DEBUG =================
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Battery: ");
  Serial.print(battVoltage);
  Serial.println(" V");

  // ================= PRIORITY 1: EMERGENCY =================
  if (digitalRead(BUTTON_PIN) == LOW) {

    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(MOTOR_PIN, HIGH);

    digitalWrite(LED_RED, (now / 200) % 2);  // kedip cepat
    return;
  }

  // ================= PRIORITY 2: BATTERY LOW =================
  if (battVoltage < LOW_BATTERY) {
    digitalWrite(LED_RED, (now / 500) % 2);  // kedip pelan
  }
  // ================= PRIORITY 3: OBSTACLE =================
  else if (distance < 100 && distance >= 20) {
    digitalWrite(LED_RED, HIGH);
  }
  else {
    digitalWrite(LED_RED, LOW);
  }

  // ================= MOTOR & BUZZER =================
  if (distance < 100) {

    distance = constrain(distance, 20, 100);

    int interval = map(distance, 20, 100, 80, 600);
    int onTime   = map(distance, 20, 100, 500, 60);

    if (!outputState && (now - lastTrigger >= interval)) {
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(MOTOR_PIN, HIGH);
      outputState = true;
      lastTrigger = now;
    }

    if (outputState && (now - lastTrigger >= onTime)) {
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(MOTOR_PIN, LOW);
      outputState = false;
      lastTrigger = now;
    }

  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(MOTOR_PIN, LOW);
  }
}
