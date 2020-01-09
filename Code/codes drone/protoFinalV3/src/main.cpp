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
// Create an instance of the LSM9DS0 library called `gyroData` the
// parameters for this constructor are:
// [SPI or I2C Mode declaration],[gyro I2C address],[xm I2C add.]
LSM9DS0 gyroData(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
float pitch, roll;
Servo esc[4];
int puissancesM[4] = {pMotor, pMotor, pMotor, pMotor};

void printOrientation(float x, float y, float z);
void setupGyro();
void loopGyro();

void setupGyro()
{
  gyroData.begin();
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
}

void loopGyro()
{
  gyroData.readAccel();
  printOrientation(gyroData.calcAccel(gyroData.ax), gyroData.calcAccel(gyroData.ay), 
                   gyroData.calcAccel(gyroData.az));
}



void printOrientation(float x, float y, float z)
{
  roll = atan2(x, sqrt(y * y) + (z * z));
  pitch = atan2(y, sqrt(x * x) + (z * z));
  pitch *= 180.0 / PI;
  roll *= 180.0 / PI;
}



void offsetMoteur1(float puissance)
{
  esc[0].writeMicroseconds(puissancesM[0] + puissance);
}

void offsetMoteur2(float puissance)
{
  esc[1].writeMicroseconds(puissancesM[1] + puissance);
}

void offsetMoteur3(float puissance)
{
  esc[2].writeMicroseconds(puissancesM[2] + puissance);
}

void offsetMoteur4(float puissance)
{
  esc[3].writeMicroseconds(puissancesM[3] + puissance);
}


bool isStabalized(float angle)
{
  return (angle > -margeAngle && angle < margeAngle);
}

void checkPitch()
{
  if(!isStabalized(pitch))
  {
    offsetMoteur1(pitch);
    offsetMoteur2(pitch);
    offsetMoteur3(-pitch);
    offsetMoteur4(-pitch);
  }
}

void checkRoll()
{
  if(!isStabalized(roll))
  {
    offsetMoteur1(roll);
    offsetMoteur3(roll);
    offsetMoteur2(-roll);
    offsetMoteur4(-roll);
  }
}
void stabilisation()
{
  loopGyro();
  checkPitch();
  checkRoll();
}

void setup() {
  //Serial.begin(9600);
  setupGyro();
  setupMotors();
}

void loop() {
  stabilisation();
}