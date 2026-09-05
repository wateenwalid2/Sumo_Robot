// ESP32 + L298N + 2 DC Motors
// Motor A = Right Motor
// Motor B = Left Motor

// Right Motor 
#define ENA 25
#define IN1 26
#define IN2 27

// Left Motor 
#define ENB 32
#define IN3 33
#define IN4 23

// PWM 
#define PWM_FREQ 5000
#define PWM_RES 8
#define SPEED 200

void setup() {

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // PWM setup
  ledcAttach(ENA, PWM_FREQ, PWM_RES);
  ledcAttach(ENB, PWM_FREQ, PWM_RES);

  stopMotors();
}

void loop() {

  // Forward
  forward(SPEED);
  delay(2000);

  // Stop
  stopMotors();
  delay(1000);

  // Backward
  backward(SPEED);
  delay(2000);

  // Stop
  stopMotors();
  delay(1000);

  // Turn Right
  turnRight(SPEED);
  delay(1000);

  // Stop
  stopMotors();
  delay(1000);

  // Turn Left
  turnLeft(SPEED);
  delay(1000);

  // Stop
  stopMotors();
  delay(2000);
}

// Forward 
void forward(int speed) {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, speed);
  ledcWrite(ENB, speed);
}

// Backward 
void backward(int speed) {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, speed);
  ledcWrite(ENB, speed);
}

// Turn Right 
void turnRight(int speed) {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, speed);
  ledcWrite(ENB, speed);
}

// Turn Left 
void turnLeft(int speed) {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, speed);
  ledcWrite(ENB, speed);
}

// Stop 
void stopMotors() {

  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}