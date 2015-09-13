#include <SoftwareSerial.h>
 
SoftwareSerial XBee =  SoftwareSerial(2, 3);

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

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
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    XBee.write((byte)0x01);
    Serial.println(buttonState);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    XBee.write((byte)0x00 );
    Serial.println(buttonState);
  }
  delay(10);
}
