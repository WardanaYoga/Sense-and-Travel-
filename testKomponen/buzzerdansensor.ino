#define TRIG 25
#define ECHO 26
#define BUZZER 32

#define CHANNEL 0
#define RESOLUTION 8

long duration;
float distance;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  ledcSetup(CHANNEL, 1000, RESOLUTION);
  ledcAttachPin(BUZZER, CHANNEL);
}

float readDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2;

  return distance;
}

void loop() {

  distance = readDistance();
  Serial.println(distance);

  if (distance <= 20 && distance > 0) {

    // Batasi jarak minimum 5 cm
    if (distance < 5) distance = 5;

    // Mapping jarak ke frekuensi
    int freq = map(distance, 20, 5, 1000, 4000);

    ledcWriteTone(CHANNEL, freq);

  } else {
    ledcWriteTone(CHANNEL, 0); // mati
  }

  delay(50);
}
