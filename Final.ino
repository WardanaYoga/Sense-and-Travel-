#define TRIG 25
#define ECHO 26
#define BUZZER 32
#define MOTOR 27

unsigned long previousMillis = 0;
unsigned long interval = 1000;

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(MOTOR, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(MOTOR, LOW);

  Serial.begin(115200);
}

float readDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000); // timeout 30ms
  float distance = duration * 0.034 / 2;

  return distance;
}

void loop() {

  float distance = readDistance();
  Serial.println(distance);

  unsigned long currentMillis = millis();

  // ===== LOGIKA JARAK =====

  if (distance > 100 || distance == 0) {
    digitalWrite(BUZZER, LOW);
    digitalWrite(MOTOR, LOW);
  }

  else if (distance > 60) {
    interval = 800;
    digitalWrite(MOTOR, LOW);
  }

  else if (distance > 30) {
    interval = 400;
    digitalWrite(MOTOR, LOW);
  }

  else if (distance > 10) {
    interval = 150;
    digitalWrite(MOTOR, HIGH);  // getar aktif
  }

  else { // < 10 cm
    digitalWrite(BUZZER, HIGH);
    digitalWrite(MOTOR, HIGH);
    return;
  }

  // ===== KONTROL BEEP NON-BLOCKING =====

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    digitalWrite(BUZZER, !digitalRead(BUZZER));
  }
}
