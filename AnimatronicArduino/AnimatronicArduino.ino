#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <AccelStepper.h>

// Initialize variables for the sensor that baselines the motors
#define sensorPin 0
int sensorLevel = 0;

// Set up motors
Adafruit_MotorShield motorShieldA(0x60);

// Position of Motor A1 relative to Sensor A in centimetres
#define motorA1x 20
#define motorA1y -5
// Motor A2
#define motorA2x -3
#define motorA2y -20

#define motorA1Steps 4096
#define motorA2Steps 200
Adafruit_StepperMotor *motorA1 = motorShieldA.getStepper(motorA1Steps, 1);
Adafruit_StepperMotor *motorA2 = motorShieldA.getStepper(motorA2Steps, 2);
// Used to control position of motor
float motorA1Pos;
float motorA2Pos;

// Set up functions that guide how the motor moves forward and backward
void forwardstep1() {
  motorA1->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {
  motorA1->onestep(BACKWARD, DOUBLE);
}
void forwardstep2() {
  motorA2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {
  motorA2->onestep(BACKWARD, DOUBLE);
}
// Add acceleration to the stepper
AccelStepper motorA1acc(forwardstep1, backwardstep1);
AccelStepper motorA2acc(forwardstep2, backwardstep2);

// Set up the range finders
#define pwPinA 7 // RF A
#define pwPinB 8 // RF B
// Distance between range finders. Used for triangulation of the person.
#define distRF 70.0
long pulseA, pulseB;

// Set up variables for triangulation
float xCoord, yCoord;
float lastAngle = 90; // Used to prevent erratic readings
double angle;


// Set up variables for smoothing the stepper motion
const int numReadings  = 3; // Number of readings to average
int readingIndex = 0; // Index of current reading
float readings[numReadings]; // Array for holding readings
float readingsTotal = 0.0; // Running total
float smoothedAngle = 0.0;

int motorA2CurrSteps = 0;


void setup() {
  Serial.begin(9600);
  motorShieldA.begin();

  // Set up the pins for the ultrasonic sensors
  pinMode(pwPinA, INPUT);
  pinMode(pwPinB, INPUT);

  // Set up the smoothing array by making it all 0s
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  motorA1acc.setSpeed(500);
  motorA2acc.setSpeed(50);
  sensorLevel = analogRead(sensorPin);
  // Serial.println(sensorLevel);
  while (sensorLevel > 5000) {
    sensorLevel = analogRead(sensorPin);
    // Serial.println(sensorLevel);
    motorA1acc.runSpeed();
    motorA2acc.runSpeed();
  }

  motorA1acc.setCurrentPosition(0);
  motorA1acc.setMaxSpeed(4000.0);
  motorA1acc.setAcceleration(600.0);
  motorA2acc.setCurrentPosition(0);
  motorA2acc.setMaxSpeed(4000.0);
  motorA2acc.setAcceleration(600.0);
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

// Calculates where to position the motor to point at the object detected.
float motorPositionDeg(float xCoord, float yCoord, float motorX, float motorY) {
  float rad, deg;
  int steps;
  // atan 2 takes the arctan and finds which quadrant it is in
  // based on the x and y values provided
  rad = atan2((yCoord - -1 * motorY), (xCoord - motorX));
  // Return angle in degrees
  deg = rad * 180 / M_PI;
  return deg;
}

float degToSteps(float posDeg, int steps) {
  float degPerStep;
  degPerStep = 360 / (float)steps;
  // Have to multiply by -1 since positive angles are clockwise
  // for the stepper library, while they are normally negative in trig.
  steps = -1 * posDeg / degPerStep;
  return steps;
}

int cycleCount = 16000;

void loop() {
  // Serial.print(", Motor A Steps: ");
  // Serial.println(motorA2Pos);

  // Sends out a pulse to find the position of the person every 16000 cycles
  // to prevent blocking.
  if (cycleCount == 16000) {
    // Get the distance from sensor A and B
    pulseA = pulseIn(pwPinA, HIGH);
    pulseA = pulseA / 10.0; // 10 uS per cm
    pulseB = pulseIn(pwPinB, HIGH);
    pulseB = pulseB / 10.0;

    // Serial.print("A: ");
    // Serial.print(pulseA);
    // Serial.print(", B: ");
    // Serial.print(pulseB);

    // Get the angle relative to the first sensor
    angle = angleCalc(pulseA, pulseB);
    // Serial.print(", Angle: ");
    //  Serial.print(angle);

    // Smooth the movement
    readingsTotal = readingsTotal - readings[readingIndex];
    readings[readingIndex] = angle;
    readingsTotal = readingsTotal + readings[readingIndex];
    readingIndex = readingIndex + 1;
    // Loop around the array
    if (readingIndex >= numReadings) {
      readingIndex = 0;
    }

    smoothedAngle = readingsTotal / (float)numReadings;

    // Find the x-coordinate and y-coordinate of the person
    xCoord = pulseA * cos(smoothedAngle);
    // All y values are negative because of the coordinate system used
    // relative to the position of the sensors.
    yCoord = -1 * pulseA * sin(smoothedAngle);
    Serial.print("X: ");
    Serial.print(xCoord);
    Serial.print(", Y: ");
    Serial.println(yCoord);
    // Serial.print(", Sensor A Angle: ");
    // Serial.print(angle);
    // Serial.print(", Smoothed: ");
    // Serial.println(smoothedAngle);

    // This part makes the motors lock on the person, and not jitter around unless
    // the person moves again.

    // Fetch how the motor should position itself
    motorA1Pos = motorPositionDeg(xCoord, yCoord, motorA1x, motorA1y);
    motorA1Pos = degToSteps(motorA1Pos, motorA1Steps / 2.0);
    motorA2Pos = motorPositionDeg(xCoord, yCoord, motorA2x, motorA2y);
    motorA2Pos = degToSteps(motorA2Pos, motorA2Steps);

    // If the position of the person has moved such that the motor
    // should move more than 3 steps (roughly 5 degrees), move the motor.
    // It's good enough to only have to check one motor - any more
    // would be redundant
    if (abs(motorA2CurrSteps - motorA2Pos) > 3) {
      motorA1acc.moveTo(motorA1Pos);
      motorA2acc.moveTo(motorA2Pos);

      motorA2CurrSteps = motorA2Pos;
      cycleCount = 0;
    } else {
      // Stops current flowing through the motor to prevent overheating
      // and keep it fixed on the person. If not enabled, a NEMA-17 motor
      // can reach over 100 degrees celcius.
      motorA1->release();
      motorA2->release();
      // Serial.println("Output disabled.");
    }

  } else {
    // Loop and increment the motor position
    cycleCount = cycleCount + 1;
    motorA1acc.run();
    motorA2acc.run();
  }
}
