#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// The unique ID of the unit
int statueId = 3;

Servo myServo;

#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

// Set up message packing

// Define the size of the buffer used to send messages
#define MESSAGE_BUFFER_SIZE   64

// Initialize the send and receive message buffers
char sendMessageBuffer[MESSAGE_BUFFER_SIZE];

String receivedMessage;
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

void pack_message(int statueId, int commandId, int angle, char audio[], char messageBuffer[]) {
  String message_builder = "";
  message_builder.concat(statueId);
  message_builder.concat(":");
  message_builder.concat(commandId);
  message_builder.concat(":");
  message_builder.concat(angle);
  message_builder.concat(":");
  message_builder.concat(audio);

  message_builder.toCharArray(messageBuffer, MESSAGE_BUFFER_SIZE);
}

void unpack_message(char messageBuffer[], int *unitIdReceived, int *commandId, int *angle, char audio[]) {
  sscanf(messageBuffer, "%d:%d:%d:%s", unitIdReceived, commandId, angle, audio);
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output

  Serial.begin(9600);
  // Serial.println("Testing motors and audio begins!");
  //Serial1.begin(9600);

  myServo.attach(2);

  if (! musicPlayer.begin()) { // initialise the music player
     // Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  // Serial.println(F("VS1053 found"));
  
  SD.begin(CARDCS);    // initialise the SD card
  
  // Set volume for left, right channels. lower numbers == louder volume!
  //musicPlayer.setVolume(20,20);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  musicPlayer.playFullFile("Left.mp3");
  //musicPlayer.playFullFile("01.mp3");
}

// the loop function runs over and over again forever
void loop() {
  if (Serial.available()) {

    receivedMessage = Serial.readString();
    receivedMessage.toCharArray(receiveMessageBuffer, MESSAGE_BUFFER_SIZE);
    unpack_message(receiveMessageBuffer, &receiveUnitId, &receiveCommandId, &receiveAngle, receiveAudio);

    musicPlayer.stopPlaying();
    myServo.write(receiveAngle);
    
    musicPlayer.startPlayingFile(receiveAudio);
  }
}
