#define TRIG 25
#define ECHO 26
#define BUZZER 32
#define MOTOR 27

#define CHANNEL 0
#define RESOLUTION 8

unsigned long previousMillis = 0;
unsigned long beepInterval = 800;
bool buzzerState = false;

float distance;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(MOTOR, OUTPUT);
  digitalWrite(MOTOR, LOW);
  ledcSetup(CHANNEL, 3000, RESOLUTION);
  ledcAttachPin(BUZZER, CHANNEL);
}

float readDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  return duration * 0.034 / 2;
}

void loop() {

  distance = readDistance();
  Serial.println(distance);

  bool objectDetected = (distance > 0 && distance <= 20);

  // ================= MOTOR =================
  digitalWrite(MOTOR, objectDetected ? HIGH : LOW);

  // ================= BUZZER =================
  if (objectDetected) {

    if (distance < 5) distance = 5;

    beepInterval = map(distance, 20, 5, 800, 100);

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= beepInterval) {
      previousMillis = currentMillis;
      buzzerState = !buzzerState;
      ledcWriteTone(CHANNEL, buzzerState ? 3000 : 0);
    }

  } else {
    ledcWriteTone(CHANNEL, 0);
  }
}
