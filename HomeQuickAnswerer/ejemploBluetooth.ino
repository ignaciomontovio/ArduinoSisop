#include <SoftwareSerial.h>

SoftwareSerial BTserial(2,3) ; //TX - RX EN PLACA BLUETOOTH

char c = ' ';

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Inicializando configuración del HC-05...");
  
  BTserial.begin(9600);
  Serial.println("Esperando comandos AT");

}

void loop() {
  // put your main code here, to run repeatedly:
  if(BTserial.available()){
    Serial.write(BTserial.read());
  }

  if(Serial.available()){
    BTserial.write(Serial.read());
  }
}