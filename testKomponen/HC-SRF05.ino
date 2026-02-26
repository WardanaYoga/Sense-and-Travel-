#define TRIG 25
#define ECHO 26

void setup() {
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  long duration;
  float distance;

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000); // tambahkan timeout!

  if (duration == 0) {
    Serial.println("Tidak ada pantulan");
  } else {
    distance = duration * 0.034 / 2;
    Serial.print("Jarak: ");
    Serial.print(distance);
    Serial.println(" cm");
  }

  delay(200);
}
