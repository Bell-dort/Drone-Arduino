#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

 
RF24 radio(7,8); //pin CE, CSN
const byte  address[6] = "00001"; //channel d'écriture/lecture
int joyStickInput[3] = {0, 0, 0};
int joyStickLeftY;
int joyStickRightX;
int joyStickRightY;

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
      joyStickInput[i] = map(joyStickInput[i], 0, 1023, -255, 255); 
    }
  joyStickLeftY = joyStickInput[0];
  joyStickRightX = joyStickInput[1];
  joyStickRightY = joyStickInput[2];
  }
}

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

void loopReception()
{
  acquisitionInput();
  affichageInput();
}

void setup() {
  Serial.begin(9600);
  setupReception();
}

void loop() {
  loopReception();
}