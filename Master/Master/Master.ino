// Serial port setup

#include <SoftwareSerial.h>
SoftwareSerial XBee =  SoftwareSerial(2, 3);

// Servo Setup
// The shield uses pins 9 and 10, exposing their PWM capabilities
int potPin = 0;
int potVal;

// Set of variables for choosing which file to play
int currentFileNumber = 99;
int fileNumber = 0;
String filename;
char filename_char[10];
bool changed = true;

// constants won't change. They're used here to
// set pin numbers:
const int ledPin =  13;      // the number of the LED pin
int angle;
int commandCount = 0;

// Define the size of the buffer used to send messages
#define MESSAGE_BUFFER_SIZE   64

// Initialize the send and receive message buffers
char sendMessageBuffer[MESSAGE_BUFFER_SIZE];
char receiveMessageBuffer[MESSAGE_BUFFER_SIZE];

// Define the size of the buffer for the payload
#define PAYLOAD_BUFFER_SIZE   64

// Used to store the received payload
char receiveAudio[PAYLOAD_BUFFER_SIZE];
// Used to store the received payload
int receiveAngle;
// Used to store the id of the unit a command was received from
int receiveUnitId;
// Used to store the id of the command acknowledgement 
int receiveCommandId;


void pack_message(int unitId, int commandId, int angle, char audio[], char messageBuffer[]) {
  String message_builder = "";
  message_builder.concat(unitId);
  message_builder.concat(":");
  message_builder.concat(commandId);
  message_builder.concat(":");
  message_builder.concat(angle);
  message_builder.concat(":");
  message_builder.concat(audio);
  message_builder.concat("@");

  message_builder.toCharArray(messageBuffer, MESSAGE_BUFFER_SIZE);
}

void unpack_message(char messageBuffer[], int *unitIdReceived, int *commandId, int *angle, char audio[]) {
  sscanf(messageBuffer, "%d:%d:%d:%s", unitIdReceived, commandId, angle, audio);
}

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("Testing Up!");
  // set the data rate for the SoftwareSerial port
  XBee.begin(9600);
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
    if (fileNumber < 10) {
      filename = "0";
      filename.concat(fileNumber);
      filename.concat(".mp3");
    } else {
      filename = "";
      filename.concat(fileNumber);
      filename.concat(".mp3");
    }
    filename.toCharArray(filename_char, 10);

    pack_message(3, commandCount, angle, filename_char, sendMessageBuffer);
    Serial.println(sendMessageBuffer);
    Serial.println("");

    XBee.print(sendMessageBuffer);
    commandCount += 1;
  }

  delay(500);
}
