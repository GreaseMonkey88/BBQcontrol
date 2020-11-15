#include <Arduino.h>
#include <Servo.h>

Servo myservo;

int incomingByte = 0; // for incoming serial data
int pos = 90;

void setup() {
  Serial.begin(9600);
  myservo.attach(9);
  myservo.write(pos);
  delay(50);
  Serial.println(pos);
}

void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    if (incomingByte == 43) {
      pos = pos + 5;
      myservo.write(pos);
      Serial.println(pos);
    }

    if (incomingByte == 45) {
      pos = pos - 5;
      myservo.write(pos);
      Serial.println(pos);
    }
  }



}