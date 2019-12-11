#include <Arduino.h>
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS0.h>

#include <Wire.h>
#include <Adafruit_MotorShield.h>


  // not used in this demo but required!

// i2c
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();

// You can also use software SPI
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(13, 12, 11, 10, 9);
// Or hardware SPI! In this case, only CS pins are passed in
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(10, 9);
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *MotorNE = AFMS.getMotor(1);
Adafruit_DCMotor *MotorNO = AFMS.getMotor(2);
Adafruit_DCMotor *MotorSO = AFMS.getMotor(3);
Adafruit_DCMotor *MotorSE = AFMS.getMotor(4);
void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS)
}

void setup() 
{
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif
  Serial.begin(9600);
  Serial.println("LSM raw read demo");
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS0. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS0 9DOF");
  Serial.println("");
  Serial.println("");

  AFMS.begin();
  MotorNO->setSpeed(100); 
  MotorNE->setSpeed(100);
  MotorSE->setSpeed(100);
  MotorSO->setSpeed(100);

}

void loop() 
{
  lsm.read();
  MotorNE->run(FORWARD);
  MotorNO->run(BACKWARD);
  MotorSO->run(FORWARD);
  MotorSE->run(BACKWARD);

  Serial.print("Gyro X: "); Serial.print((int)lsm.gyroData.x);   Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.gyroData.y);        Serial.print(" \n");
  
  delay(200);
}