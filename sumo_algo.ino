
#include <Arduino.h>
#include <Wire.h>

#define LEFT_EN   25
#define LEFT_IN1  26
#define LEFT_IN2  27

#define RIGHT_EN  32
#define RIGHT_IN1 33
#define RIGHT_IN2 23

#define PWM_FREQ 5000
#define PWM_RES  8

#define FRONT_ARRAY_1  -1
#define FRONT_ARRAY_2  -1
#define FRONT_ARRAY_3  -1

#define FRONT_LEFT_CORNER  -1
#define FRONT_RIGHT_CORNER -1

#define REAR_ARRAY_1  -1
#define REAR_ARRAY_2  -1
#define REAR_ARRAY_3  -1

#define REAR_LEFT_CORNER  -1
#define REAR_RIGHT_CORNER -1

#define TOF_SDA_1 21
#define TOF_SCL_1 22

#define TOF_SDA_2 18
#define TOF_SCL_2 19

#define TOF_RX 16
#define TOF_TX 17

#define TOF_ADDRESS 0x52

int distanceFront = 999;
int distanceLeft  = 999;
int distanceRight = 999;

#define SEARCH_SPEED       130
#define APPROACH_SPEED     180
#define ATTACK_SPEED       240
#define FULL_ATTACK_SPEED  255

#define ESCAPE_SPEED       220
#define ESCAPE_TURN_SPEED  220

#define OPPONENT_DETECT_DISTANCE 120
#define ATTACK_DISTANCE           50
#define FULL_ATTACK_DISTANCE      20

#define WHITE_LEVEL HIGH

enum RobotState
{
  SEARCHING,

  TARGET_LEFT,
  TARGET_FRONT,
  TARGET_RIGHT,

  ESCAPE_FRONT,
  ESCAPE_LEFT,
  ESCAPE_RIGHT,
  ESCAPE_REAR
};

RobotState currentState = SEARCHING;

unsigned long stateStartTime = 0;

unsigned long searchStartTime = 0;

unsigned long searchDirectionStartTime = 0;

bool searchDirectionLeft = true;


void setup()
{
  Serial.begin(115200);

  pinMode(LEFT_EN, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  pinMode(RIGHT_EN, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  pinMode(FRONT_ARRAY_1, INPUT);
  pinMode(FRONT_ARRAY_2, INPUT);
  pinMode(FRONT_ARRAY_3, INPUT);

  pinMode(FRONT_LEFT_CORNER, INPUT);
  pinMode(FRONT_RIGHT_CORNER, INPUT);

  pinMode(REAR_ARRAY_1, INPUT);
  pinMode(REAR_ARRAY_2, INPUT);
  pinMode(REAR_ARRAY_3, INPUT);

  pinMode(REAR_LEFT_CORNER, INPUT);
  pinMode(REAR_RIGHT_CORNER, INPUT);

  Wire.begin(TOF_SDA_1, TOF_SCL_1);
  Wire1.begin(TOF_SDA_2, TOF_SCL_2);

  Serial2.begin(
    9600,
    SERIAL_8N1,
    TOF_RX,
    TOF_TX
  );

  currentState = SEARCHING;

  stateStartTime = millis();

  searchStartTime = millis();

  searchDirectionStartTime = millis();

  stopMotors();

  Serial.println("=================================");
  Serial.println("SUMO ROBOT STARTED");
  Serial.println("=================================");
  Serial.println("10 EDGE SENSORS");
  Serial.println("Front array  = 3");
  Serial.println("Front corners = 2");
  Serial.println("Rear array   = 3");
  Serial.println("Rear corners = 2");
  Serial.println("=================================");
}


void loop()
{
  int edgeState = checkEdges();

  if (edgeState != 0)
  {
    handleEdge(edgeState);

    return;
  }

  readToFs();

  determineTarget();

  executeState();
}


int read_TOF_I2C_1()
{
  unsigned short dist = 0;

  Wire.beginTransmission(TOF_ADDRESS);

  Wire.write(0x00);

  Wire.endTransmission();

  Wire.requestFrom(TOF_ADDRESS, 2);

  if (Wire.available() != 2)
  {
    return -1;
  }

  dist = Wire.read() << 8;

  dist |= Wire.read();

  return dist;
}


int read_TOF_I2C_2()
{
  unsigned short dist = 0;

  Wire1.beginTransmission(TOF_ADDRESS);

  Wire1.write(0x00);

  Wire1.endTransmission();

  Wire1.requestFrom(TOF_ADDRESS, 2);

  if (Wire1.available() != 2)
  {
    return -1;
  }

  dist = Wire1.read() << 8;

  dist |= Wire1.read();

  return dist;
}


int readTOF_UART()
{
  if (Serial2.available() > 0)
  {
    int dist = Serial2.parseInt();

    return dist;
  }

  return -1;
}


void readToFs()
{
  int d1 = read_TOF_I2C_1();

  int d2 = read_TOF_I2C_2();

  int d3 = readTOF_UART();

  if (d1 >= 0)
    distanceFront = d1 / 10;
  else
    distanceFront = 999;

  if (d2 >= 0)
    distanceLeft = d2 / 10;
  else
    distanceLeft = 999;

  if (d3 >= 0)
    distanceRight = d3 / 10;
  else
    distanceRight = 999;
}


int checkEdges()
{
  int front1 = digitalRead(FRONT_ARRAY_1);

  int front2 = digitalRead(FRONT_ARRAY_2);

  int front3 = digitalRead(FRONT_ARRAY_3);

  int frontLeft = digitalRead(FRONT_LEFT_CORNER);

  int frontRight = digitalRead(FRONT_RIGHT_CORNER);

  int rear1 = digitalRead(REAR_ARRAY_1);

  int rear2 = digitalRead(REAR_ARRAY_2);

  int rear3 = digitalRead(REAR_ARRAY_3);

  int rearLeft = digitalRead(REAR_LEFT_CORNER);

  int rearRight = digitalRead(REAR_RIGHT_CORNER);


  if (front1 == WHITE_LEVEL ||
      front2 == WHITE_LEVEL ||
      front3 == WHITE_LEVEL ||
      frontLeft == WHITE_LEVEL ||
      frontRight == WHITE_LEVEL)
  {
    return 1;
  }


  if (frontLeft == WHITE_LEVEL ||
      rearLeft == WHITE_LEVEL)
  {
    return 2;
  }


  if (frontRight == WHITE_LEVEL ||
      rearRight == WHITE_LEVEL)
  {
    return 3;
  }


  if (rear1 == WHITE_LEVEL ||
      rear2 == WHITE_LEVEL ||
      rear3 == WHITE_LEVEL ||
      rearLeft == WHITE_LEVEL ||
      rearRight == WHITE_LEVEL)
  {
    return 4;
  }


  return 0;
}


void handleEdge(int edge)
{
  stopMotors();

  delay(20);

  switch (edge)
  {
    case 1:
      currentState = ESCAPE_FRONT;
      break;

    case 2:
      currentState = ESCAPE_LEFT;
      break;

    case 3:
      currentState = ESCAPE_RIGHT;
      break;

    case 4:
      currentState = ESCAPE_REAR;
      break;
  }

  stateStartTime = millis();

  executeState();
}


void determineTarget()
{
  bool frontDetected =
    distanceFront <= OPPONENT_DETECT_DISTANCE;

  bool leftDetected =
    distanceLeft <= OPPONENT_DETECT_DISTANCE;

  bool rightDetected =
    distanceRight <= OPPONENT_DETECT_DISTANCE;


  if (!frontDetected &&
      !leftDetected &&
      !rightDetected)
  {
    currentState = SEARCHING;

    return;
  }


  if (frontDetected &&
      distanceFront <= distanceLeft &&
      distanceFront <= distanceRight)
  {
    currentState = TARGET_FRONT;
  }

  else if (leftDetected &&
           distanceLeft <= distanceRight)
  {
    currentState = TARGET_LEFT;
  }

  else if (rightDetected)
  {
    currentState = TARGET_RIGHT;
  }
}


void executeState()
{
  switch (currentState)
  {
    case SEARCHING:
      searchOpponent();
      break;

    case TARGET_LEFT:
      attackLeft();
      break;

    case TARGET_FRONT:
      attackFront();
      break;

    case TARGET_RIGHT:
      attackRight();
      break;

    case ESCAPE_FRONT:
      escapeFront();
      break;

    case ESCAPE_LEFT:
      escapeLeft();
      break;

    case ESCAPE_RIGHT:
      escapeRight();
      break;

    case ESCAPE_REAR:
      escapeRear();
      break;
  }
}


void searchOpponent()
{
  unsigned long now = millis();


  if (now - searchStartTime < 1500)
  {
    driveForward(SEARCH_SPEED);

    return;
  }


  if (now - searchDirectionStartTime >= 900)
  {
    searchDirectionLeft = !searchDirectionLeft;

    searchDirectionStartTime = now;
  }


  if (searchDirectionLeft)
  {
    turnLeft(SEARCH_SPEED);
  }

  else
  {
    turnRight(SEARCH_SPEED);
  }
}


void attackLeft()
{
  if (distanceLeft <= FULL_ATTACK_DISTANCE)
  {
    setMotors(
      FULL_ATTACK_SPEED,
      FULL_ATTACK_SPEED
    );
  }

  else if (distanceLeft <= ATTACK_DISTANCE)
  {
    setMotors(
      ATTACK_SPEED,
      FULL_ATTACK_SPEED
    );
  }

  else
  {
    setMotors(
      APPROACH_SPEED,
      ATTACK_SPEED
    );
  }
}


void attackFront()
{
  if (distanceFront <= FULL_ATTACK_DISTANCE)
  {
    setMotors(
      FULL_ATTACK_SPEED,
      FULL_ATTACK_SPEED
    );
  }

  else if (distanceFront <= ATTACK_DISTANCE)
  {
    setMotors(
      ATTACK_SPEED,
      ATTACK_SPEED
    );
  }

  else
  {
    setMotors(
      APPROACH_SPEED,
      APPROACH_SPEED
    );
  }
}


void attackRight()
{
  if (distanceRight <= FULL_ATTACK_DISTANCE)
  {
    setMotors(
      FULL_ATTACK_SPEED,
      FULL_ATTACK_SPEED
    );
  }

  else if (distanceRight <= ATTACK_DISTANCE)
  {
    setMotors(
      FULL_ATTACK_SPEED,
      ATTACK_SPEED
    );
  }

  else
  {
    setMotors(
      ATTACK_SPEED,
      APPROACH_SPEED
    );
  }
}


void escapeFront()
{
  unsigned long elapsed =
    millis() - stateStartTime;


  if (elapsed < 220)
  {
    driveBackward(ESCAPE_SPEED);
  }

  else if (elapsed < 570)
  {
    turnRight(ESCAPE_TURN_SPEED);
  }

  else
  {
    currentState = SEARCHING;

    searchStartTime = millis();

    searchDirectionStartTime = millis();

    stopMotors();
  }
}


void escapeLeft()
{
  unsigned long elapsed =
    millis() - stateStartTime;


  if (elapsed < 220)
  {
    driveBackward(ESCAPE_SPEED);
  }

  else if (elapsed < 570)
  {
    turnRight(ESCAPE_TURN_SPEED);
  }

  else
  {
    currentState = SEARCHING;

    searchStartTime = millis();

    searchDirectionStartTime = millis();

    stopMotors();
  }
}


void escapeRight()
{
  unsigned long elapsed =
    millis() - stateStartTime;


  if (elapsed < 220)
  {
    driveBackward(ESCAPE_SPEED);
  }

  else if (elapsed < 570)
  {
    turnLeft(ESCAPE_TURN_SPEED);
  }

  else
  {
    currentState = SEARCHING;

    searchStartTime = millis();

    searchDirectionStartTime = millis();

    stopMotors();
  }
}


void escapeRear()
{
  unsigned long elapsed =
    millis() - stateStartTime;


  if (elapsed < 250)
  {
    driveForward(ESCAPE_SPEED);
  }

  else if (elapsed < 600)
  {
    turnLeft(ESCAPE_TURN_SPEED);
  }

  else
  {
    currentState = SEARCHING;

    searchStartTime = millis();

    searchDirectionStartTime = millis();

    stopMotors();
  }
}


void setLeftMotor(int speed)
{
  speed = constrain(speed, -255, 255);


  if (speed > 0)
  {
    digitalWrite(LEFT_IN1, HIGH);

    digitalWrite(LEFT_IN2, LOW);

    analogWrite(LEFT_EN, speed);
  }


  else if (speed < 0)
  {
    digitalWrite(LEFT_IN1, LOW);

    digitalWrite(LEFT_IN2, HIGH);

    analogWrite(LEFT_EN, -speed);
  }


  else
  {
    digitalWrite(LEFT_IN1, LOW);

    digitalWrite(LEFT_IN2, LOW);

    analogWrite(LEFT_EN, 0);
  }
}


void setRightMotor(int speed)
{
  speed = constrain(speed, -255, 255);


  if (speed > 0)
  {
    digitalWrite(RIGHT_IN1, HIGH);

    digitalWrite(RIGHT_IN2, LOW);

    analogWrite(RIGHT_EN, speed);
  }


  else if (speed < 0)
  {
    digitalWrite(RIGHT_IN1, LOW);

    digitalWrite(RIGHT_IN2, HIGH);

    analogWrite(RIGHT_EN, -speed);
  }


  else
  {
    digitalWrite(RIGHT_IN1, LOW);

    digitalWrite(RIGHT_IN2, LOW);

    analogWrite(RIGHT_EN, 0);
  }
}


void setMotors(int leftSpeed, int rightSpeed)
{
  setLeftMotor(leftSpeed);

  setRightMotor(rightSpeed);
}


void driveForward(int speed)
{
  setMotors(
    speed,
    speed
  );
}


void driveBackward(int speed)
{
  setMotors(
    -speed,
    -speed
  );
}


void turnLeft(int speed)
{
  setMotors(
    -speed,
    speed
  );
}


void turnRight(int speed)
{
  setMotors(
    speed,
    -speed
  );
}


void stopMotors()
{
  setMotors(
    0,
    0
  );
}

