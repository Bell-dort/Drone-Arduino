#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

 
RF24 radio(14,32);
const byte  address[6] = "00001";
uint16_t joyStickInput[4] = {0, 0, 0, 0};
uint16_t joyStickLeftX;
uint16_t joyStickLeftY;
uint16_t joyStickRightX;
uint16_t joyStickRightY;

void setup() {
  Serial.begin(9600);
  radio.begin();

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if(!radio.available())
  {
    Serial.println("no data");
  }
  else
  {
    radio.read(joyStickInput, sizeof(joyStickInput));
    for(int i = 0; i<=3; i++)
    {
      joyStickInput[i] = map(joyStickInput[i], 0, 1023, 0, 255);
    }
    joyStickLeftX = joyStickInput[0];
    joyStickLeftY = joyStickInput[1];
    joyStickRightX = joyStickInput[2];
    joyStickRightY = joyStickInput[3];
    Serial.print("Joystick 1 X : ");
    Serial.print(joyStickLeftX);
    Serial.print("  Joystick 2 X : ");
    Serial.println(joyStickRightX);
    Serial.print("JoyStick 1 Y : ");
    Serial.print(joyStickLeftY);
    Serial.print("  JoyStick 2 Y : ");
    Serial.println(joyStickRightY);
    Serial.println("");
  }

}