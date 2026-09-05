// 3-Channel IR Line Sensor Test

#define IR_LEFT    32
#define IR_CENTER  33
#define IR_RIGHT   25

void setup() {
  Serial.begin(115200);

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);

  Serial.println("3-Channel IR Sensor Test");
}

void loop() {

  int left   = digitalRead(IR_LEFT);
  int center = digitalRead(IR_CENTER);
  int right  = digitalRead(IR_RIGHT);

  Serial.print("L: ");
  Serial.print(left);

  Serial.print("   C: ");
  Serial.print(center);

  Serial.print("   R: ");
  Serial.println(right);

  delay(200);
}