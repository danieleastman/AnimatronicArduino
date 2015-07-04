#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <AccelStepper.h>

// Used for the sensor that baselines the motors
const int sensorPin = 0;
int sensorLevel = 0;
Adafruit_MotorShield motorShieldA(0x60);

// Set up motors

// Position of Motor A relative to Sensor A
#define motorA2x 30
#define motorA2y 0

Adafruit_StepperMotor *motorA2 = motorShieldA.getStepper(200, 2);
int motorA2Angle, motorA2Steps;
void forwardstep2() {  
  motorA2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {  
  motorA2->onestep(BACKWARD, DOUBLE);
}
AccelStepper motorA2acc(forwardstep2, backwardstep2);

// Set up the range finders
#define pwPinA 7 // RF A
#define pwPinB 8 // RF B
// Distance between range finders
#define distRF 71.0
long pulseA, pulseB;

// Set up variables for triangulation
float xCoord, yCoord;
float lastAngle = 90; // Used to prevent erratic readings
double angle;

void setup() {
  Serial.begin(9600);
  motorShieldA.begin();

  // Set up the pins for the ultrasonic sensors
  pinMode(pwPinA, INPUT);
  pinMode(pwPinB, INPUT);

  motorA2acc.setSpeed(50);
  sensorLevel = analogRead(sensorPin);
  Serial.println(sensorLevel);
  while(sensorLevel > 5000) {
      sensorLevel = analogRead(sensorPin);
      Serial.println(sensorLevel);
      motorA2acc.runSpeed();
  }

  motorA2acc.setCurrentPosition(0);
  motorA2acc.setMaxSpeed(4000.0);
  motorA2acc.setAcceleration(600.0);
}

float angleCalc(float distA, float distB) {
  float angleRad;
  // Law of Cosines rearranged
  angleRad = acos((distA*distA + distRF*distRF - distB*distB) / (2 * distA * distRF));
  
  // Prevent erratic readings
  if (isnan(angleRad) == 1) {
    return lastAngle;
  } else {
    lastAngle = angleRad;
    return angleRad;
  }
}

int motorPosition(float xCoord, float yCoord) {
  float rad, deg;
  int steps;
  // atan 2 takes the arctan and finds which quadrant it is in
  // based on the x and y values provided
  rad = atan2((yCoord - -1*motorA2y), (xCoord - motorA2x));
  // Return angle in degrees
  deg = rad * 180 / M_PI;
  // Have to multiply by -1 since positive angles are clockwise for the stepper library, while they are normally negative in trig.
  steps = -1 * deg / 1.8;
  return steps;
}

int cycleCount = 16000;

void loop() {  
  // Serial.print(", Motor A Steps: ");
  // Serial.println(motorA2Angle);
  if (cycleCount == 16000){
    // Get the distance from sensor A
    pulseA = pulseIn(pwPinA, HIGH);
    pulseA = pulseA / 10.0; // 10 uS per cm
    pulseB = pulseIn(pwPinB, HIGH);
    pulseB = pulseB / 10.0;
    
    // Serial.print("A: ");
    // Serial.print(pulseA); 
    // Serial.print(", B: ");
    // Serial.print(pulseB);
    
    angle = angleCalc(pulseA, pulseB);
    // Serial.print(", Angle: ");
    //  Serial.print(angle);
    
    xCoord = pulseA*cos(angle);
    // All y values are negative because of the coordinate system used relative to the position of the sensors.
    yCoord = -1*pulseA*sin(angle);
    // Serial.print("X: ");
    // Serial.print(xCoord); 
    // Serial.print(", Y: ");
    // Serial.print(yCoord);
    // Serial.print(", Sensor A Angle: ");
    // Serial.print(angle);
    
    motorA2Angle = motorPosition(xCoord, yCoord);
    motorA2acc.moveTo(motorA2Angle);
    cycleCount = 0;
  } else {
    cycleCount = cycleCount + 1;
    motorA2acc.run();
  }
}
