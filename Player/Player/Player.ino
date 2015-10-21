
#include <SPI.h>
#include <SD.h>

#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin

#define MAX_LINE_LENGTH 128
#define MAX_ARG_LENGTH 32



void parse_line_args(char line[], char statue_id[], char command[], char command_args[],  char pause[]) {
  int i = 0;
  int arg_index = 0;
  int arg_id = 0;  
  
  char c = line[i];
  while (c != 0x0) {
    if (c == 0x3A) {
      // arg delimiter (colon)
      
      

      // terminate arg with null
      if (arg_id == 0) {
        // statue_id
        statue_id[arg_index] = 0x0;
      }
      else if (arg_id == 1) {
        // command
        command[arg_index] = 0x0;
      }
      else if (arg_id == 2) {
        // command args
        command_args[arg_index] = 0x0;
      }
      else if (arg_id == 3) {
        // pause before next command
        pause[arg_index] = 0x0;
      }

      arg_index = 0;
      arg_id++;
    }
    else {

      // save character into appropriate arg
      if (arg_id == 0) {
        // statue_id
        statue_id[arg_index] = c;
      }
      else if (arg_id == 1) {
        // command
        command[arg_index] = c;
      }
      else if (arg_id == 2) {
        // command args
        command_args[arg_index] = c;
      }
      else if (arg_id == 3) {
        // pause before next command
        pause[arg_index] = c;
      }

      arg_index++;
    }
    
    i++;
    c = line[i];
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing SD card...");
  SD.begin(CARDCS);    // initialise the SD card
  Serial.println("SD card initialized.");

  File scriptFile = SD.open("script.txt");

  if (scriptFile && scriptFile.available()) {
    char line[MAX_LINE_LENGTH];
    char statue_id[MAX_ARG_LENGTH];
    char command[MAX_ARG_LENGTH];
    char command_args[MAX_ARG_LENGTH];
    char pause[MAX_ARG_LENGTH];
    
    int i = 0;
    char c = scriptFile.read();
    while (c != EOF) {

      if (c == 0xA) {
        // end of line, parse and save
        line[i] = 0x0;
        i = 0;
        Serial.println(line);

        parse_line_args(line, statue_id, command, command_args, pause);
        Serial.print("  "); Serial.println(statue_id);
        Serial.print("  "); Serial.println(command);
        Serial.print("  "); Serial.println(command_args);
        Serial.print("  "); Serial.println(pause);
        Serial.println();
      }
      else {
        line[i] = (char) c;
        i++;
      }
      
      c = scriptFile.read();
      
    }
    scriptFile.close();
  }
  else {
    Serial.println("error opening script.txt");
  }
}


void loop() {

}
