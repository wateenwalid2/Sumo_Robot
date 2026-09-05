#include <Wire.h>

#define SDA_1 21
#define SCL_1 22

#define SDA_2 18
#define SCL_2 19

#define RX 16
#define TX 17

#define TOF_ADDRESS 0x52 

void setup() {
  Serial.begin(9600);
  Wire.begin(SDA_1, SCL_1);
  Wire1.begin(SDA_2, SCL_2);
  Serial2.begin(9600, SERIAL_8N1, RX, TX);
}



void loop() {

  int distance_1 = read_TOF_I2C_1();
  if (distance_1 != -1) {
    Serial.print("TOF 1 : ");
    Serial.print(distance_1 / 10.0);
    Serial.println(" cm");
  }

  int distance_2  = read_TOF_I2C_2();
  if (distance_2 != -1) {
    Serial.print("TOF 2 : ");
    Serial.print(distance_2 / 10.0);
    Serial.println(" cm");
  }

  int distance_3 = read_TOF_UART();
  if (distance_3  != -1) {
    Serial.print("TOF 3 : ");
    Serial.print(distance_3  / 10.0);
    Serial.println(" cm");
  }
  
  delay(100);
}


int read_TOF_I2C_1(){
  unsigned short dist = 0;
  Wire.beginTransmission(TOF_ADDRESS);
  Wire.write(0x00); 
  Wire.endTransmission();
  
  Wire.requestFrom(TOF_ADDRESS, 2);
  if(Wire.available() != 2){
    return -1;
  } 
  
  dist = Wire.read() << 8; 
  dist |= Wire.read();     
  return dist;             
}

int read_TOF_I2C_2(){
  unsigned short dist = 0;
  Wire1.beginTransmission(TOF_ADDRESS);
  Wire1.write(0x00); 
  Wire1.endTransmission();
  
  Wire1.requestFrom(TOF_ADDRESS, 2);
  if(Wire1.available() != 2){
    return -1;
  } 
  
  dist = Wire1.read() << 8; 
  dist |= Wire1.read();     
  return dist;             
}


int readTOF_UART() {
  if (Serial2.available() > 0) {
    int dist = Serial2.parseInt();
    return dist;
  }else{
    return -1;
  }
  
}