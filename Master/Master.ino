// Serial port setup

#include <SoftEasyTransfer.h>
#include <SoftwareSerial.h>
#include <Metro.h>
SoftwareSerial XBee =  SoftwareSerial(2, 3);

// SoftEasyTransfer ETin, ETout; 
SoftEasyTransfer ETout;

// Used for scheduling distance readings and communications
Metro getDistance = Metro(100); // Take a reading every 100ms
Metro sendCoords = Metro(250);  // Send the result every 250ms


// Set of variables for choosing which file to play
//int currentFileNumber = 99;
//int fileNumber = 0;
//bool changed = true;

// Distance between the range finders
#define distRF 148.0
#define rangeA 7 // RF A
#define rangeB 8 // RF B
long pulseA, pulseB;

// Set up variables needed to smooth readings
float lastAngle = 90; // Used to prevent erratic readings
double angle;
// Set up variables for smoothing the stepper motion
const int numReadings  = 3; // Number of readings to average
int readingIndex = 0; // Index of current reading
float readings[numReadings]; // Array for holding readings
float readingsTotal = 0.0; // Running total
float smoothedAngle = 0.0;

// Set up variables for triangulation
float xCoord, yCoord;

#define NUM_STATUE 1
int statues[] = {1};

struct SEND_DATA_STRUCTURE{
  int unitId;
  int commandType;
  int xPos;
  int yPos;
  // int audio;
};
SEND_DATA_STRUCTURE sendData;

// struct RECEIVE_DATA_STRUCTURE{
//   int blink;
// };
// RECEIVE_DATA_STRUCTURE receiveData;

void setup() {  
  Serial.begin(9600);
  Serial.println("Testing Up!");

  // Set up the pins for the ultrasonic sensors
  pinMode(rangeA, INPUT);
  pinMode(rangeB, INPUT);
  // set the data rate for the SoftwareSerial port
  XBee.begin(9600);

  ETout.begin(details(sendData), &XBee);
  // ETin.begin(details(receiveData), &XBee);

  // Set up the smoothing array by making it all 0s
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
    }
  }

// Calculates the angle of the tracked object relative to the first sensor
float angleCalc(float distA, float distB) {
  float angleRad;
  // Law of Cosines rearranged
  angleRad = acos((distA * distA + distRF * distRF - distB * distB) / (2 * distA * distRF));

  // Prevent erratic readings
  if (isnan(angleRad) == 1) {
    return lastAngle;
  } else {
    lastAngle = angleRad;
    return angleRad;
  }
}

void loop() {
  // Calculate the distance reading at the interval specified above
  if (getDistance.check() == 1) {

    // Get the distance from sensor A and B
    pulseA = pulseIn(rangeA, HIGH);
    pulseA = pulseA / 10.0; // 10 uS per cm
    pulseB = pulseIn(rangeB, HIGH);
    pulseB = pulseB / 10.0;

    // Get the angle relative to the first sensor
    angle = angleCalc(pulseA, pulseB);

    // Smooth the movement
    readingsTotal = readingsTotal - readings[readingIndex]; // Subtract the old
    readings[readingIndex] = angle; // Insert the new
    readingsTotal = readingsTotal + readings[readingIndex]; // Add the new
    readingIndex = readingIndex + 1; // Move to the next index
    // Loop around the array
    if (readingIndex >= numReadings) {
      readingIndex = 0; // Start the index back at 0
    }
    // Take the average of the last three readings
    smoothedAngle = readingsTotal / (float)numReadings;
    // Find the x-coordinate and y-coordinate of the person. All y values are
    // negative because of the coordinate system used relative to the position
    // of the sensors.
    xCoord = pulseA * cos(smoothedAngle);
    yCoord = -1 * pulseA * sin(smoothedAngle);
  }

  // Send the smoothed distance reading at the rate specified above
  if (sendCoords.check() == 1) {
    for(int i = 0; i < NUM_STATUE; i++) {
      sendData.unitId = statues[i];
      sendData.commandType = 1;
      sendData.xPos = xCoord;
      Serial.print(sendData.xPos);
      sendData.yPos = yCoord;
      Serial.print(", ");
      Serial.println(sendData.yPos);
      ETout.sendData();
      // Serial.println("---------------");
      // Serial.print("Statue: ");
      // Serial.println(statues[i]);
      // Serial.print("Command: ");
      // Serial.println("MoveTo");
      // Serial.print("X: ");
      // Serial.println(xCoord);
      // Serial.print(", Y: ");
      // Serial.println(yCoord);
      // Serial.println("---------------");
    }
  }

  // Generate the file number to play from the angle of the pot
  // fileNumber = constrain(map(angle, 0, 180, 1, 12), 1, 11);
  // if (currentFileNumber != fileNumber) {
  //   changed = true;
  //   currentFileNumber = fileNumber;
  // } else {
  //   changed = false;
  // }

}

