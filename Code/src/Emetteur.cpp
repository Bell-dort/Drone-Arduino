#include <Arduino.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

#include <SPI.h>

RF24 radio(7,8);
const byte  address[6] = "00001";
uint16_t joyStickInput[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  radio.begin();
  //radio.setRetries(15,15);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  
  joyStickInput[0] = analogRead(A0);
  joyStickInput[1] = analogRead(A1);
  joyStickInput[2] = analogRead(A2);
  joyStickInput[3] = analogRead(A3);
  Serial.print("Joystick 1 X : ");
  Serial.print(joyStickInput[0]);
  Serial.print("  Joystick 2 X : ");
  Serial.println(joyStickInput[2]);
  Serial.print("JoyStick 1 Y : ");
  Serial.print(joyStickInput[1]);
  Serial.print("  JoyStick 2 Y : ");
  Serial.println(joyStickInput[3]);
  Serial.println("");
  radio.write(joyStickInput, sizeof(joyStickInput));
  

}