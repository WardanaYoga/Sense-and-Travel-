#define BUZZER 32

void setup() {
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  digitalWrite(BUZZER, HIGH);  // nyala
  delay(1000);
  digitalWrite(BUZZER, LOW);   // mati
  delay(1000);
}
