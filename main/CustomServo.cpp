/**
 * Custom Servo Library Implementation
 * 
 * Uses incremental step-based movement for smooth, non-jerky servo control.
 * servo.write() sets an absolute angle
 *  Instead, update() nudges the servo one step per call toward the target, 
 * relying on potentiometer feedback to confirm actual position before each step.
 */

#include "Arduino.h"
#include "CustomServo.h"

CustomServo::CustomServo()
    : servoPin(0), feedbackPin(0), calibLow(0), calibHigh(1023),
      targetAngle(90), currentAngle(90), commandedAngle(90), lastTime(0)
{
}

CustomServo::~CustomServo()
{
}

void CustomServo::begin(uint8_t servo_pin, uint8_t feedback_pin, int calib_low, int calib_high)
{
    servoPin     = servo_pin;
    feedbackPin  = feedback_pin;
    calibLow     = calib_low;
    calibHigh    = calib_high;

    pinMode(feedbackPin, INPUT);

    servo.attach(servoPin);
    servo.write(90);

    // Sync internal state to actual position
    currentAngle  = getCurrentAngle();
    targetAngle   = currentAngle;
    commandedAngle = currentAngle;

    lastTime = millis();
}

uint8_t CustomServo::getCurrentAngle()
{
    int reading = analogRead(feedbackPin);
    // Map the analog reading (calibrated range) to 0-180 degrees
    uint8_t angle = map(reading, calibLow, calibHigh, 0, 180);
    return angle;
}

/*
Claude review:
targetAngle member is never set from outside
The private targetAngle member is initialised to 90 in the constructor, but update() takes a target parameter and never stores it into targetAngle. 
This makes isAtTarget() always compare against the constructor-initialised value (90), not whatever you last passed into update(). 
Either remove targetAngle from the member variables and fix isAtTarget() to take a parameter, or update targetAngle at the top of update(). 

Also, isAtTarget is never used, I think update() should poll isAtTarget every iteration.
*/
void CustomServo::update(uint8_t target, bool debug = false)
{
    // Enforce a minimum interval between steps to let the servo physically move
    // and for the potentiometer reading to settle before the next nudge.
    unsigned long now = millis();
    if (now - lastTime < STEP_INTERVAL_MS) return;
    lastTime = now;

    currentAngle = getCurrentAngle();

    int error = (int)target - (int)currentAngle;
    if (isAtTarget(target)) return;

    // Step toward target but never past it (Not sure about this logic)
    if (error > 0)
        commandedAngle = min(commandedAngle + STEP_SIZE, (int)target);
    else
        commandedAngle = max(commandedAngle - STEP_SIZE, (int)target);

    commandedAngle = constrain(commandedAngle, 0, 180);

    if (debug) {
    // Temporary for debugging, I KNOW YOU SHOULDNT PRINT SO MUCH
    Serial.print("Target: ");
    Serial.print(target);
    Serial.print(" | Current: ");
    Serial.print(currentAngle);
    Serial.print(" | Error: ");
    Serial.print(error);
    Serial.print(" | Writing: ");
    Serial.println(commandedAngle);
   //#endif
    }
    servo.write(commandedAngle);
}

bool CustomServo::isAtTarget(uint8_t target)
{
    return abs((int)targetAngle - (int)currentAngle) <= TOLERANCE;
}
