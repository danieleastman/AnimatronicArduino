#include <SPI.h>
#include <SD.h>
#include <Adafruit_VS1053.h>

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

//void unpack_message(char messageBuffer[], int *unitIdReceived, int *commandId, int *angle, char audio[]) {
//  sscanf(messageBuffer, "%d:%d:%d:%s", unitIdReceived, commandId, angle, audio);
//}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  Serial.println("Testing motors and audio begins!");
  
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
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
  musicPlayer.playFullFile("Left.mp3");
  delay(2000);
//  
//  if (Serial.available()) {
//
//    receivedMessage = Serial.readStringUntil('@');
//    Serial.println(receivedMessage);
//    receivedMessage.toCharArray(receiveMessageBuffer, MESSAGE_BUFFER_SIZE);
//    unpack_message(receiveMessageBuffer, &receiveUnitId, &receiveCommandId, &receiveAngle, receiveAudio);
//    
////    Serial.println(receiveUnitId);
////    Serial.println(receiveCommandId);
////    Serial.println(receiveAngle);
////    Serial.println(receiveAudio);
////    Serial.println("");
//
//    musicPlayer.stopPlaying();
//    myServo.write(receiveAngle);
//    
//    musicPlayer.startPlayingFile(receiveAudio);
//  }
}
