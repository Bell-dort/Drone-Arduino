#include <Arduino.h>
#include <SPI.h> // Included for SFE_LSM9DS0 library
#include <Wire.h>
#include <SFE_LSM9DS0.h>
#include <Servo.h>
#include "ESC.h"

#define pMotor 1300
#define margeAngle 1

// SDO_XM and SDO_G are both grounded, so our addresses are:
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 infoGyro(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
Servo esc[4];

void setupGyro()
{
  infoGyro.begin();
}

void loopGyro()
{
  infoGyro.readGyro();
}

void motorsSpeedReset()
{
    for(int i = 0; i < 4; i++)
    {
        esc[i].writeMicroseconds(pMotor);
    }
}

void setupMotors()
{
  esc[0].attach(10);
  esc[1].attach(9);
  esc[2].attach(5);
  esc[3].attach(3);
  esc[0].writeMicroseconds(1000);
  esc[1].writeMicroseconds(1000);
  esc[2].writeMicroseconds(1000);
  esc[3].writeMicroseconds(1000);

  delay(2000);

  motorsSpeedReset();
}

void upMotor(Servo premierM, Servo deuxiemeM, float gyro)
{
  premierM.writeMicroseconds(pMotor + gyro);
  deuxiemeM.writeMicroseconds(pMotor + gyro);
}

void downMotor(Servo premierM, Servo deuxiemeM, float gyro)
{
  premierM.writeMicroseconds(pMotor - gyro);
  deuxiemeM.writeMicroseconds(pMotor - gyro);
}

void test()
{
  loopGyro();
  if(infoGyro.gy > margeAngle)
  {
    upMotor(esc[0], esc[1], infoGyro.gy);
    downMotor(esc[2], esc[3], infoGyro.gy);
  }
  else if(infoGyro.gy < -margeAngle)
  {
    upMotor(esc[2], esc[3], infoGyro.gy);
    downMotor(esc[0],esc[1], infoGyro.gy);
  }
  if(infoGyro.gx > margeAngle)
  {
    upMotor(esc[1], esc[3], infoGyro.gx);
    downMotor(esc[2], esc[4], infoGyro.gx);
  }
  else if(infoGyro.gx < -margeAngle)
  {
    upMotor(esc[2], esc[4], infoGyro.gx);
    downMotor(esc[1], esc[3], infoGyro.gx);
  }
}
void setup() {
  setupGyro();
  setupMotors();
}

void loop() {
  test();
}