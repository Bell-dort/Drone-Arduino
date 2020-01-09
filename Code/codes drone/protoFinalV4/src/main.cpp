#include <Arduino.h>
#include <SPI.h> // Included for SFE_LSM9DS0 library
#include <Wire.h>
#include <SFE_LSM9DS0.h>
#include <Servo.h>
#include "ESC.h"
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>



//constantes du gyroscope
// SDO_XM and SDO_G are both grounded, so our addresses are:
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW

//variable du gyroscope
LSM9DS0 gyroData(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 
float pitch, roll;

//variables du nRF
RF24 radio(7,8); //pin CE, CSN
const byte  address[6] = "00001"; //channel d'écriture/lecture
int joyStickInput[3] = {0, 0, 0}; //tableau de récupération des valeurs de joystick
//variables avec des noms plus explicites
int joyStickLeftY;
int joyStickRightX;
int joyStickRightY;

//puissance des moteurs
#define pMotor 1300 //puissance des moteurs au démarrage
#define margeAngle 7 
Servo esc[4];
int puissancesM[4] = {pMotor, pMotor, pMotor, pMotor}; 

//Fonctions du nRF (récéption des valeurs)
void setupReception();
void acquisitionInput();

//Fonctions du gyroscope
void setupGyro();
void printOrientation(float x, float y, float z);
void loopGyro();

//Fonctions de controle des moteurs
void setupMotors();
void offsetMoteur1(float puissance);
void offsetMoteur2(float puissance);
void offsetMoteur3(float puissance);
void offsetMoteur4(float puissance);

//Fonctions de stabilisation du drone
void stabilisation();
bool isStabalized(float angle);
void checkPitch();
void checkRoll();


void affichageInput()
{
  
  Serial.print("  Joystick 2 X : ");
  Serial.println(joyStickRightX);
  Serial.print("JoyStick 1 Y : ");
  Serial.print(joyStickLeftY);
  Serial.print("  JoyStick 2 Y : ");
  Serial.println(joyStickRightY);
  Serial.println("");
}


//nRF
void setupReception()
{
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void acquisitionInput()
{
  if(radio.available())
  {
    radio.read(joyStickInput, sizeof(joyStickInput));
    for(int i = 0; i<=2; i++) //recuperation des données de l'émetteur
                              //et mapping des valeurs pour réutilisation de celles-ci
    {
      joyStickInput[i] = map(joyStickInput[i], 0, 1023, -50, 50); 
    }
  joyStickLeftY = joyStickInput[0];
  joyStickRightX = joyStickInput[1];
  joyStickRightY = joyStickInput[2];
  }
}

//Gyroscope
void setupGyro()
{
  gyroData.begin();
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
  Serial.print("pitch "); Serial.println(pitch);
  Serial.print("roll "); Serial.println(roll);
}

//Moteurs
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

//Gestion des 4 moteurs de manière indépendante
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


void testJoystick()
{
  acquisitionInput();
  //affichageInput();
  delay(20);
  for(int i=0; i<4; i++)
  {
    if(puissancesM[i] < 1200)
    {
      puissancesM[i] = 1200;
      esc[i].writeMicroseconds(puissancesM[i]);
    }
    else if(puissancesM[i] > 1500)
    {
      puissancesM[i] = 1500;
      esc[i].writeMicroseconds(puissancesM[i]);
    }
    else
    {
      puissancesM[i] = puissancesM[i] + joyStickLeftY/30;
      esc[i].writeMicroseconds(puissancesM[i]);
    }
    
  }
}

//Stabilisation du drone
void stabilisation()
{
  loopGyro();
  checkPitch();
  checkRoll();
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

bool isStabalized(float angle)
{
  return (angle > -margeAngle && angle < margeAngle);
}


void setup() {
  Serial.begin(9600);
  setupGyro();
  setupMotors();
  setupReception();
}

void loop() {
  testJoystick();
  stabilisation();
}