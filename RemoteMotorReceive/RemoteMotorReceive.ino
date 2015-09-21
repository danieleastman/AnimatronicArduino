

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
#include <Servo.h>
//#include <VarSpeedServo.h>


#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


Servo myServo;
SoftwareSerial XBee = SoftwareSerial(8,9);

int angle;


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

int current_file_number = 99;
int file_number = 0;
String filename;
char filename_char[10];
bool changed = true;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output

  Serial.begin(9600);
  Serial.println("Testing motors and audio begins!");
  XBee.begin(9600);

  myServo.attach(2);

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

void padding( int number, byte width ) {
  int currentMax = 10;
  for (byte i=1; i<width; i++){
    if (number < currentMax) {
      Serial.print("0");
    }
    currentMax *= 10;
  }
  Serial.print(number);
} 

// the loop function runs over and over again forever
void loop() {
  if (XBee.available()) {
    angle = (int)XBee.read();
    Serial.print("Received: ");
    Serial.println(angle);
    myServo.write(angle);
    file_number = constrain(map(angle, 0, 180, 1, 12), 1, 11);
    
    if (file_number < 10) {
      filename = "0";
      filename.concat(file_number);
      filename.concat(".mp3");
    }
    else {
      filename = "";
      filename.concat(file_number);
      filename.concat(".mp3");
    }
    
    if (current_file_number != file_number) {
      changed = true;
      current_file_number = file_number;
    }
    else {
      changed = false;
    }

    if (changed == true) {
      Serial.println("SHIT CHANGED, YO!");
      //if (musicPlayer.playingMusic == true) {
        musicPlayer.stopPlaying();
      //}
      Serial.println("Donkey!");
      filename.toCharArray(filename_char, 10);
      Serial.println(filename_char);
      musicPlayer.startPlayingFile(filename_char);
    }
  }
}
