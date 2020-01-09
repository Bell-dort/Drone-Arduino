#include <Arduino.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

RF24 radio(7,8); //pin CE,CSN
const byte  address[6] = "00001"; //channel d'écriture/lecture
//l'envoi des données se fait par l'intermédiaire d'un entier à 16 bits au lieu d'un int, car
//l'envoie de 4 int en tableau est trop volumineux
uint16_t joyStickInput[3] = {0, 0, 0};

void setupEmission()
{
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void affichageInput()
{
  Serial.print("  Joystick 2 X : ");
  Serial.println(joyStickInput[1]);
  Serial.print("JoyStick 1 Y : ");
  Serial.print(joyStickInput[0]);
  Serial.print("  JoyStick 2 Y : ");
  Serial.println(joyStickInput[2]);
  Serial.println("");
}

void loopEmission()
{
  //Acquisition des valeurs de chaque axe des joysticks
  joyStickInput[0] = analogRead(A0);
  joyStickInput[1] = analogRead(A3);
  joyStickInput[2] = analogRead(A2);
  //Affichage des valeurs
  affichageInput(); 
  //Envoi des valeurs
  radio.write(joyStickInput, sizeof(joyStickInput));
}
void setup() {
  Serial.begin(9600);
  setupEmission();
}

void loop() {
  loopEmission();
}