//         .~* INSTRUCTIONS *~.
//
//              MODE 0 = OFF
//
//          MODE 1 = PULLING
//
//             MODE 2 = PAUSE
//
//          MODE 3 = PUSHING
//
//             MODE 4 = PAUSE
//
//          MODE 5 = STAGNANT PULSE
//
//              MODE 6 = PAUSE
//  
//          MODE 7 = ROTATING PULSE
//      
//
//           SEND 'm' INTO THE SERIAL MONITOR TO SWITH TO THE NEXT MODE
//           THE PROGRAM WILL COMPLETE THE CYCLE OF THE PREVIOUS MODE BEFORE SWITCHING TO THE NEXT AFTER YOU SEND 'm' 
//           !!!! SO DO NOT SEND 'm' MORE THAN ONCE IF IT DOES NOT SWITCH MODES IMMEDIATELY AFTER SENDING 'm' !!!!
//    
//         SENDING 'm' AFTER MORE 4 WILL START THE PROGRAM BACK AT MODE 0
    



// L293D Motor Driver Pins
const int enablePin = 4;  // PWM pin for speed control (can be any PWM pin)
const int in1Pin = 6;     // Input 1 pin
const int in2Pin = 5;     // Input 2 pin

int MODE = 0;

unsigned long electromagnetOnTime = 200;  // Time to keep electromagnets on in milliseconds
unsigned long electromagnetOffTime = 200; // Time to keep electromagnets off in milliseconds
unsigned long fastelectromagnetOnTime = 20;  // Time to keep electromagnets on in milliseconds
unsigned long fastelectromagnetOffTime = 20; // Time to keep electromagnets off in milliseconds
unsigned long previousMillis = 0;        // Variable to store the last time electromagnets were updated



const unsigned long moveDelay = 1000;    // Delay between stepper movements in milliseconds
const unsigned long pulseDuration = 500;  // Duration of the pulsefast loop in milliseconds
unsigned long pulseStartTime = 0;         // Variable to store the start time of the pulsefast loop
unsigned long moveStartTime = 0;          // Variable to store the start time of the stepper movement
unsigned int forwardMoveCount = 0;           // Counter for forward movements
const unsigned int forwardMovementCount = 9;



bool electromagnetState = false;  // Track the current state of the electromagnets

#include <Arduino.h>
#define MOTOR_STEPS 200
#define RPM 10

// Define stepper motor connections and motor interface type. 
// Motor interface type must be set to 1 when using a driver
#define DIR 8
#define STEP 9
#define SLEEP A5

 #include "A4988.h"
 #define MS1 A0
 #define MS2 A1
 #define MS3 A2
 A4988 stepper(MOTOR_STEPS, DIR, STEP, SLEEP, MS1, MS2, MS3);



void setup() {

  Serial.begin(9600);
  
  // Set the motor control pins as outputs
  pinMode(enablePin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);

//  pinMode(BUTTON_PIN, INPUT);

  stepper.begin(RPM);
  stepper.enable();
  MagnetOff();    
}





void loop() {

 stepper.setMicrostep(16);  // Set microstep mode to 1:16

 
  if (Serial.available() > 0) {
    char inputChar = Serial.read();

    // Check if the received character is 'M'
    if (inputChar == 'm') {
      MODE++;  // Increment the count
      
      // Check if count reaches 4, then reset it to 0
      if (MODE > 7) {
        MODE = 0;
       }
    }
 }

 
// MODE 0 ///// off
 if (MODE == 0) {
      Serial.print("MODE: ");
      Serial.println(MODE);
MagnetOff();
      }



// MODE 1 (Pulling Circle)
      if (MODE == 1) {
      Serial.print("MODE: ");
      Serial.println(MODE);
      PositiveElectroMagnet();
      stepper.move(16 * MOTOR_STEPS);    // reverse revolution
       PositiveElectroMagnet();
      delay(1000);
      stepper.move(-16 * MOTOR_STEPS);    // Backward motion
      delay(500);
      }
      
// MODE 0 ///// off
 if (MODE == 2) {
      Serial.print("MODE: ");
      Serial.println(MODE);
MagnetOff();
      }

// MODE 2 (Pushing Circle)
      if (MODE == 3) {
      Serial.print("MODE: ");
      Serial.println(MODE);
      NegativeElectroMagnet();
      stepper.move(16 * MOTOR_STEPS);    // reverse revolution
       NegativeElectroMagnet();
      delay(1000);
      stepper.move(-16 * MOTOR_STEPS);    // Backward motion
      delay(500);
      }

// MODE 0 ///// off
 if (MODE == 4) {
      Serial.print("MODE: ");
      Serial.println(MODE);
MagnetOff();
      }

// MODE 3 (Pulsing no circle)
      if (MODE == 5) {
       Serial.print("MODE: ");
      Serial.println(MODE);
      pulsefast();
      }


      // MODE 0 ///// off
 if (MODE == 6) {
      Serial.print("MODE: ");
      Serial.println(MODE);
MagnetOff();
      }

// MODE 4 (Pulsing Circle)
 if (MODE == 7) {
  Serial.print("MODE: ");
  Serial.println(MODE);

  // Perform the action if the specified duration has passed
  if (millis() - pulseStartTime < pulseDuration) {
    pulsefast();
  } else {
    // Reset the start time for the next pulsefast loop
    pulseStartTime = millis();
    
    // Check if it's time to perform the next stepper movement
    if (millis() - moveStartTime >= moveDelay) {
      // Reset the start time for the next stepper movement
      moveStartTime = millis();

      // Perform the motor movement
      if (forwardMoveCount < forwardMovementCount) {
        stepper.move(1.78 * MOTOR_STEPS);    // forward revolution
        forwardMoveCount++;
      } else {
        MagnetOff();
        stepper.move(-16 * MOTOR_STEPS);    // backward motion
        forwardMoveCount = 0;  // Reset the counter for forward movements
      }
    }
  }
}

}








/////// Electromagnetic field in direction 1
void PositiveElectroMagnet() {
  // Enable the motor driver and set the direction for the electromagnet
  digitalWrite(enablePin, HIGH);
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
}


/////// Electromagnetic field opposite direction
void NegativeElectroMagnet() {
  // Disable the motor driver to turn off the electromagnet
  digitalWrite(enablePin, HIGH);
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
}



/////// Electromagnetic field switches every 200 ms
void pulse(){
  unsigned long currentMillis = millis();

  // Check if it's time to change the electromagnet state
  if (currentMillis - previousMillis >= (electromagnetState ? electromagnetOnTime : electromagnetOffTime)) {
    // Toggle the electromagnet state
    electromagnetState = !electromagnetState;

    // Update the electromagnet state
    if (electromagnetState) {
      PositiveElectroMagnet();
    } else {
      NegativeElectroMagnet();
    }

    // Update the last time electromagnets were changed
    previousMillis = currentMillis;
  }
}


/////// Electromagnetic field switches every 20 ms
void pulsefast(){
  unsigned long currentMillis = millis();

  // Check if it's time to change the electromagnet state
  if (currentMillis - previousMillis >= (electromagnetState ? fastelectromagnetOnTime : fastelectromagnetOffTime)) {
    // Toggle the electromagnet state
    electromagnetState = !electromagnetState;

    // Update the electromagnet state
    if (electromagnetState) {
      PositiveElectroMagnet();
    } else {
      NegativeElectroMagnet();
    }

    // Update the last time electromagnets were changed
    previousMillis = currentMillis;
  }
}



/////// Turn Off
void MagnetOff() {
  // Disable the motor driver to turn off the electromagnet
  digitalWrite(enablePin, LOW);
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
}
