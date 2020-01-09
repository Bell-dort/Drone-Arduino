#include <Arduino.h>
#include <SPI.h> // Included for SFE_LSM9DS0 library
#include <Wire.h>
#include <SFE_LSM9DS0.h>
#include <Servo.h>
#include "ESC.h"

#define pMotor 1300
#define margeAngle 7

// SDO_XM and SDO_G are both grounded, so our addresses are:
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 infoGyro(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
Servo esc[4];

// SDO_XM and SDO_G are both grounded, so our addresses are:
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
// Create an instance of the LSM9DS0 library called `gyroData` the
// parameters for this constructor are:
// [SPI or I2C Mode declaration],[gyro I2C address],[xm I2C add.]
LSM9DS0 gyroData(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
float pitch, roll;

void printOrientation(float x, float y, float z);
void setupGyro();
void loopGyro();

void setupGyro()
{
  gyroData.begin();
}

void loopGyro()
{
  gyroData.readAccel();
  printOrientation(gyroData.calcAccel(gyroData.ax), gyroData.calcAccel(gyroData.ay), 
                   gyroData.calcAccel(gyroData.az));
  Serial.println();
}



void printOrientation(float x, float y, float z)
{ 
  pitch = atan2(y, sqrt(x * x) + (z * z));
  roll = atan2(x, sqrt(y * y) + (z * z));
  pitch *= 180.0 / PI;
  roll *= 180.0 / PI;
  
  /*Serial.print("Pitch, Roll: ");
  Serial.print(pitch, 2);
  Serial.print(", ");
  Serial.println(roll, 2);*/
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
  //attribution des moteurs aux pins de la carte
  esc[0].attach(10);
  esc[1].attach(9);
  esc[2].attach(5);
  esc[3].attach(3);
  //initialisation des moteurs (1000 = valeur d'initialisation)
  esc[0].writeMicroseconds(1000);
  esc[1].writeMicroseconds(1000);
  esc[2].writeMicroseconds(1000);
  esc[3].writeMicroseconds(1000);

  delay(2000); //nécessaire à l'initialisation

  motorsSpeedReset();
}

void upMotor(Servo premierM, Servo deuxiemeM, float gyroAngle)
{
  premierM.writeMicroseconds(pMotor + gyroAngle);
  deuxiemeM.writeMicroseconds(pMotor + gyroAngle);
}

void downMotor(Servo premierM, Servo deuxiemeM, float gyroAngle)
{
  premierM.writeMicroseconds(pMotor + gyroAngle);
  deuxiemeM.writeMicroseconds(pMotor + gyroAngle);
}

void stabilisation()
{
  loopGyro();
    if(roll > margeAngle)
  {
    upMotor(esc[0], esc[2], roll);
    downMotor(esc[1], esc[3], -roll);
  }
  else if(roll < -margeAngle)
  {
    upMotor(esc[1], esc[3], -roll);
    downMotor(esc[0], esc[2], roll);
  }
  if(pitch > margeAngle)
  {
    upMotor(esc[0], esc[1], pitch);
    downMotor(esc[2], esc[3], pitch);
  }
  else if(pitch < -margeAngle)
  {
    upMotor(esc[2], esc[3], pitch);
    downMotor(esc[0],esc[1], pitch);
  }
  

}
void setup() {
  //Serial.begin(9600);
  setupGyro();
  setupMotors();
}

void loop() {
  stabilisation();
}