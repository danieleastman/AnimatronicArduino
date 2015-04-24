#include <Servo.h>

// Servo A
#define servAPin 11

// Position of Servo A relative to Sensor A
#define servAx 14.0
#define servAy 2.0
Servo servoA;
int servoAAngle;

// Range Finder A
#define pwPinA 7
// Range Finder B
#define pwPinB 8
// Distance between range finders
#define distRF 35.0



long pulseA, pulseB;
float xCoord, yCoord;
float lastAngle = 90; // Used to prevent erratic readings
double angle;

void setup() {
  Serial.begin(9600);
  servoA.attach(servAPin);
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

int servoAngle(float xCoord, float yCoord) {
  float rad, deg;
  // atan 2 takes the arctan and finds which quadrant it is in
  // based on the x and y values provided
  rad = atan2((yCoord - servAy), (xCoord - servAx));
  // Return angle in degrees
  deg = rad * 180 / M_PI;
  // The baseline for the servo is 180 degrees rotated;
  return deg;
}

void loop() {
  pinMode(pwPinA, INPUT);
  pinMode(pwPinB, INPUT);
  
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
  yCoord = pulseA*sin(angle);
  Serial.print("X: ");
  Serial.print(xCoord); 
  Serial.print(", Y: ");
  Serial.print(yCoord);
  
  servoAAngle = servoAngle(xCoord, yCoord);
  servoA.write(servoAAngle);
  Serial.print(", Servo Angle: ");
  Serial.println(servoAAngle);

  delay(500);
}
