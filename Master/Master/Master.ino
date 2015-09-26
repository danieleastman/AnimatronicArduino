// Serial port setup

#include <SoftEasyTransfer.h>
#include <SoftwareSerial.h>
SoftwareSerial XBee =  SoftwareSerial(2, 3);
SoftEasyTransfer ETin, ETout; 

// Servo Setup
// The shield uses pins 9 and 10, exposing their PWM capabilities
int potPin = 0;
int potVal;

// Set of variables for choosing which file to play
int currentFileNumber = 99;
int fileNumber = 0;
bool changed = true;

// constants won't change. They're used here to
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin
int angle;
int commandCount = 0;

struct SEND_DATA_STRUCTURE{
  int unitId;
  int commandId;
  int angle;
  int audio;
};
SEND_DATA_STRUCTURE sendData;

struct RECEIVE_DATA_STRUCTURE{
  int blink;
};
RECEIVE_DATA_STRUCTURE receiveData;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Testing Up!");
  // set the data rate for the SoftwareSerial port
  XBee.begin(9600);

  ETout.begin(details(sendData), &XBee);
  ETin.begin(details(receiveData), &XBee);
}

void padding( int number, byte width ) {
  int currentMax = 10;
  for (byte i=1; i<width; i++){
    if (number < currentMax) {
      //Serial.print("0");
    }
    currentMax *= 10;
  }
  //Serial.print(number);
} 

void loop() {  
  potVal = analogRead(potPin);
  // Serial.println(potVal);
  angle = constrain(map(potVal, 0, 1024, 0, 180), 5, 175);

  // Generate the file number to play from the angle of the pot
  fileNumber = constrain(map(angle, 0, 180, 1, 12), 1, 11);
  if (currentFileNumber != fileNumber) {
    changed = true;
    currentFileNumber = fileNumber;
  } else {
    changed = false;
  }

  if (changed == true) {
    sendData.unitId = 3;
    sendData.commandId = commandCount;
    sendData.angle = angle;
    sendData.audio = fileNumber;
    Serial.println(fileNumber);
    ETout.sendData();
    
    commandCount += 1;
  }

  if (ETin.receiveData()) {
      digitalWrite(ledPin, HIGH);
      delay(50);
      digitalWrite(ledPin, LOW);
  }

  delay(50);
}
