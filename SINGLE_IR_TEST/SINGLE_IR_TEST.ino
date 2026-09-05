#define IR_PIN 25

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
}

void loop() {
  int ir = digitalRead(IR_PIN);

  Serial.print("IR = ");
  Serial.println(ir);

  delay(100);
}