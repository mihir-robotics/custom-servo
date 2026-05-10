// Code to control customised SG90 Servo with smooth PID-based movement
#include <Servo.h>
#include "CustomServo.h"

// Defining the lowest and highest voltage values for potentiometer calibration 
// Manually measured using debug_sweep.ino, will be refactored into a calibrate() later
#define CALIBRATION_LOW 64
#define CALIBRATION_HIGH 557

// CustomServo object for PID-controlled feedback
CustomServo serv;

// Declaring read and write pins for servo on board
// On Arduino Nano, use a PWM-capable digital pin for the servo and an analog pin for feedback
uint8_t servoPin = 3;  // D3
uint8_t readPin = A0;   // A0

uint8_t targetAngle = 0;

//  Setting up the connections
void setup()
{
    Serial.begin(9600);

    serv.begin(servoPin, readPin,CALIBRATION_LOW,CALIBRATION_HIGH); // testing auto calibration
    Serial.println("Servo attached!");

    Serial.print("current angle reading:");
    Serial.println((int)serv.getCurrentAngle());
    
}

// Main loop with smooth servo movement
void loop()
{
    // Check for new target from Serial Monitor
    if (Serial.available() > 0)
    {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0)
        {
            int value = input.toInt();
            if (value >= 0 && value <= 180)
            {
                targetAngle = (uint8_t)value;
                Serial.print("New target set: ");
                Serial.println(targetAngle);
            }
            else
            {
                Serial.println("Invalid angle. Enter a value between 0 and 180.");
            }
        }
    }

    serv.update(targetAngle); // set debug flag to true to print out the values
}
