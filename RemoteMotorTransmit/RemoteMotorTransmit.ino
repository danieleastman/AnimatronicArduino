// Serial port setup

#include <SoftwareSerial.h>
SoftwareSerial XBee =  SoftwareSerial(2, 3);

// Servo Setup
// The shield uses pins 9 and 10, exposing their PWM capabilities
int potPin = 0;
int potVal;

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
int angle;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  Serial.begin(9600);
  Serial.println("Testing Up!");
  // set the data rate for the SoftwareSerial port
  XBee.begin(9600);
}

void loop() {

  potVal = analogRead(potPin);
  angle = constrain(map(potVal, 0, 1024, 0, 180), 5, 175);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  XBee.write((int)angle);
  Serial.println(angle);
  delay(100);
}
