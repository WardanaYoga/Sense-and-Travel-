#define TRIG 25
#define ECHO 26
#define BUZZER 32
#define MOTOR 27
#define BUTTON 33

unsigned long previousMillis = 0;
unsigned long interval = 1000;

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

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

  long duration = pulseIn(ECHO, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  return distance;
}

void loop() {

  // ===== PRIORITAS PANIC BUTTON =====
  if (digitalRead(BUTTON) == LOW) {  // ditekan
    digitalWrite(BUZZER, HIGH);
    digitalWrite(MOTOR, HIGH); // opsional
    return;  // abaikan sistem lain
  }

  float distance = readDistance();
  unsigned long currentMillis = millis();

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
    digitalWrite(MOTOR, HIGH);
  }

  else {
    digitalWrite(BUZZER, HIGH);
    digitalWrite(MOTOR, HIGH);
    return;
  }

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(BUZZER, !digitalRead(BUZZER));
  }
}
