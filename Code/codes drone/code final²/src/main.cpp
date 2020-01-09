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
#define coefficientAngles 1.3

//variable du gyroscope
LSM9DS0 gyroData(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 
float pitch, roll;

//variables du nRF
RF24 radio(7,8); //pin CE, CSN
const byte  address[6] = "00001"; //channel d'écriture/lecture
int joyStickInput[3] = {0, 0, 0}; //tableau de récupération des valeurs de joystick
//variables avec des noms plus explicites
float joyStickLeftY;
float joyStickRightX;
float joyStickRightY;

//variables définissant la puissance des moteurs
#define pMotorMin 1250
#define pMotorMax 1600
#define pMotor 1300 //puissance des moteurs au démarrage
#define mapJoystickGauche 50 //intensité des moteurs pilotés grâce au joystick gauche
#define mapJoystickDroit 75 //intensité des moteurs pilotés grâce au joystick droit
#define coefficientJoystickGauche 7 //coefficient influant sur la puissance commune aux moteurs
#define coefficientJoystickDroit 1  //coefficient influant sur l'angle imposé au drone
Servo esc[4];
float puissanceCommune = pMotor;

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
void gaz();
bool checkMax();
bool checkMin();

//Fonctions de stabilisation du drone
void stabilisation();
bool isStabalized(float angle);
void correction();

//Fonction à supprimer dans le code final
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


///////////////////////////Fonctions du nRF////////////////////////////////////////////////////////////
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
      joyStickInput[i] = map(joyStickInput[i], 0, 1023, -mapJoystickDroit, mapJoystickDroit); 
    }
    joyStickLeftY = joyStickInput[0];
    joyStickLeftY = map(joyStickLeftY, -mapJoystickDroit, mapJoystickDroit, -mapJoystickGauche, mapJoystickGauche);
    joyStickRightX = joyStickInput[1];
    joyStickRightY = joyStickInput[2];
  }
}

///////////////////////////Fonctions du Gyroscope/////////////////////////////////////////////////////
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
  pitch *= coefficientAngles;
  roll *= coefficientAngles;
}

//////////////////////////////////////Fonctions des moteurs/////////////////////////////////////////
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

void offsetMoteur1(float offset)
{
  esc[0].writeMicroseconds(puissanceCommune + offset);
}

void offsetMoteur2(float offset)
{
  esc[1].writeMicroseconds(puissanceCommune  + offset);
}

void offsetMoteur3(float offset)
{
  esc[2].writeMicroseconds(puissanceCommune  + offset);
}

void offsetMoteur4(float offset )
{
  esc[3].writeMicroseconds(puissanceCommune  + offset);
}

//Gestion de la puissance commune aux 4 moteurs (joystick Gauche)
void gaz()
{
  if(!checkMax() && !checkMin())
  {
    puissanceCommune = puissanceCommune + joyStickLeftY/coefficientJoystickGauche;
  }
}

bool checkMax()
{
  if(puissanceCommune > pMotorMax)
  {
    puissanceCommune = pMotorMax;
    return true;
  }
  return false;
}

bool checkMin()
{
  if(puissanceCommune < pMotorMin)
  {
    puissanceCommune = pMotorMin;
    return true;
  }
  return false;
}

/////////////////////////////////////////Stabilisation du drone/////////////////////////////////////////
void stabilisation()
{
  loopGyro();
  correction();
}

void correction()
{
  offsetMoteur1(pitch + roll - (joyStickRightX + joyStickRightY)/coefficientJoystickDroit);
  offsetMoteur2(pitch - roll + (joyStickRightX - joyStickRightY)/coefficientJoystickDroit);
  offsetMoteur3(-pitch + roll - (joyStickRightX - joyStickRightY)/coefficientJoystickDroit);
  offsetMoteur4(-pitch - roll + (joyStickRightX + joyStickRightY)/coefficientJoystickDroit);
}

////////////////////////////////////////////Main/////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  setupGyro();
  setupMotors();
  setupReception();
}

void loop() {
  acquisitionInput();
  delay(50);
  gaz();
  stabilisation();
}