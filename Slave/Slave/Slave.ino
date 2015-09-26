#include <EasyTransfer.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// The unique ID of the unit
int statueId = 3;

Servo myServo;

// Set up the audio card
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

String filename;
int fileNumber;
char filename_char[10];

// Set up easy transfer
EasyTransfer ETin, ETout;
struct RECEIVE_DATA_STRUCTURE{
  int unitId;
  int commandId;
  int angle;
  int audio;
};
RECEIVE_DATA_STRUCTURE receiveData;

struct SEND_DATA_STRUCTURE{
  int blink;
};
SEND_DATA_STRUCTURE sendData;

void setup() {
  Serial.begin(9600);
  ETin.begin(details(receiveData), &Serial);
  ETout.begin(details(sendData), &Serial);

  myServo.attach(2);

  if (! musicPlayer.begin()) { // initialise the music player
     // Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  
  SD.begin(CARDCS);    // initialise the SD card

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  musicPlayer.playFullFile("Left.mp3");
}

// the loop function runs over and over again forever
void loop() {
  if (ETin.receiveData()) {

    // musicPlayer.stopPlaying();
    myServo.write(receiveData.angle);
    sendData.blink = 1;
    ETout.sendData();

    fileNumber = receiveData.audio;

    if (fileNumber < 10) {
      filename = "0";
      filename.concat(fileNumber);
      filename.concat(".mp3");
    } else {
      filename = "";
      filename.concat(fileNumber);
      filename.concat(".mp3");
    }

    // Serial.println(filename);
    filename.toCharArray(filename_char, 10);
    // Serial.println(filename_char);
    //musicPlayer.startPlayingFile(filename_char);
  }
}
