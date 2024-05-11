#include <AccelStepper.h> // Include the AccelStepper library

// Define pins for proximity sensors---------------------------------------------------------------------------------------------------------------------------------------------------------
const int torqueSensorPin = 13;// Torque sensor pin
const int keySensorPin = 12;// Key sensor pin
const int gripperRelay = 4;
const int lockSensor = A1;
const int lockRelay = A3;
const int shackleSensorPin = A5;// Shackle sensor pin
const int shackleRelayPin = A2;// Shackle relay pin
// Define pins for stepper motor control-----------------------------------------------------------------------------------------------------------------------------------------------------
const int stepPin1 = 5;// Step pin for stepper motor 1
const int dirPin1 = 2;// Direction pin for stepper motor 1
const int stepPin2 = 3;// Step pin for stepper motor 2
const int dirPin2 = 6;// Direction pin for stepper motor 2
bool shackleSensorTriggered = false;
// Create instances of AccelStepper for stepper motor control-------------------------------------------------------------------------------------------------------------------------------
AccelStepper stepper1(1,2,5);
AccelStepper stepper2(1,3,6);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define variables to track stepper motor angle
int angle = 0;

// Variable to track if the sequence has started
bool sequenceStarted = false;

// Variable to track if the key sensor was previously triggered
bool keySensorPreviouslyTriggered = false;

// Variable to track if the stepper motor has been activated after key sensor trigger
bool stepperActivated = false;
bool jawRelayState = false;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Set pin modes for sensors
  pinMode(torqueSensorPin, INPUT);
  pinMode(keySensorPin, INPUT);
  pinMode(lockSensor,INPUT);
  pinMode(gripperRelay,OUTPUT);
  pinMode(lockRelay,OUTPUT);
  // Set pin modes for stepper motor control
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Set initial step and direction for stepper motors
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);
  digitalWrite(gripperRelay,HIGH);
  digitalWrite(lockRelay,HIGH);
  // Set stepper motor speed and acceleration
  stepper1.setMaxSpeed(900);
  stepper1.setAcceleration(300);
  stepper1.setCurrentPosition(0);
  stepper2.setMaxSpeed(900);
  stepper2.setAcceleration(300);
  stepper2.setCurrentPosition(0);
   // Set pin mode for shackle sensor
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Set pin mode for shackle relay
  pinMode(shackleRelayPin, OUTPUT);

  // Initialize shackle relay to OFF
  digitalWrite(shackleRelayPin, HIGH);
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Read sensor inputs
  int lockSensorState = digitalRead(lockSensor);
  int torqueSensorState = digitalRead(torqueSensorPin);
  int keySensorState = digitalRead(keySensorPin);

// If lock sensor is triggered and sequence has not started, start the sequence
 if (lockSensorState == HIGH && !sequenceStarted) {
    jawRelayState = true;
    digitalWrite(lockRelay, LOW); // Turn on jaw relay
 }
 
   if (keySensorState == HIGH && !keySensorPreviouslyTriggered) {
    keySensorPreviouslyTriggered = true;
    stepperActivated = false;}

// If key sensor was triggered previously and is now released, reset the flag
  if (keySensorPreviouslyTriggered && keySensorState == HIGH) {
    keySensorPreviouslyTriggered = false;
  }

// Read shackle sensor input
  int shackleSensorState = digitalRead(shackleSensorPin);

Serial.print(digitalRead(sequenceStarted));
// If key sensor is triggered and sequence has started, perform actions
  if (keySensorState == LOW && !stepperActivated) {
    // Start stepper motor with the ball screw
    sequenceStarted = true;
    delay(1000);
    digitalWrite(gripperRelay,LOW);
    stepper1.move(-800); // Assuming 1000 steps
    stepper1.runToPosition();
    stepperActivated = true; // Set stepper activation flag
  }
  
  if (torqueSensorState == LOW && sequenceStarted) {
    // Rotate stepper motor 2 by 90 degrees clockwise
    delay(600);
    stepper2.move(60); // 400 steps = 90 degrees
    stepper2.runToPosition();
    delay(500);
    digitalWrite(shackleRelayPin,LOW);
    delay(2000);
    // Return stepper motor 2 to its original position
    stepper2.move(-63); // 
    stepper2.runToPosition();
    delay(500);
     digitalWrite(shackleRelayPin,HIGH);
    delay(1000);
     stepper1.move(800); // Assuming 1000 steps
    stepper1.runToPosition(); 
  }
// If the ball screw motor has returned to its original position, turn off gripper relay
if (stepper1.currentPosition() == 0 && sequenceStarted) {
    digitalWrite(gripperRelay, HIGH);// Turn off gripper relay
    digitalWrite(lockRelay,HIGH);
    sequenceStarted = false; 
}
    
}
