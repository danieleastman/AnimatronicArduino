

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
#include <SoftwareSerial.h>
// #include <Servo.h>
#include <VarSpeedServo.h>

VarSpeedServo myServo;
SoftwareSerial XBee = SoftwareSerial(2, 3);

int angle;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output

  Serial.begin(9600);
  Serial.println("Testing begins!");
  XBee.begin(9600);

  myServo.attach(13);
}

// the loop function runs over and over again forever
void loop() {
  if (XBee.available()) {
    angle = (int)XBee.read();
    Serial.print("Received: ");
    Serial.println(angle);
    myServo.write(angle, 30, true);
  }
}
