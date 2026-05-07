/**
 * Custom Servo Library Implementation
 * 
 * Uses incremental step-based movement for smooth, non-jerky servo control.
 * servo.write() sets an absolute angle — not a velocity or force — so PID
 * output cannot be meaningfully applied to it. Instead, update() nudges the
 * servo one step per call toward the target, relying on potentiometer feedback
 * to confirm actual position before each step.
 */

#include "Arduino.h"
#include "CustomServo.h"

#define STEP_SIZE 2

CustomServo::CustomServo()
    : servoPin(0), feedbackPin(0), calibLow(70), calibHigh(590),
      targetAngle(90), currentAngle(90), lastTime(0)
{
}

CustomServo::~CustomServo()
{
}

void CustomServo::begin(uint8_t servo_pin, uint8_t feedback_pin, int calib_low, int calib_high)
{
    servoPin = servo_pin;
    feedbackPin = feedback_pin;
    calibLow = calib_low;
    calibHigh = calib_high;

    // Initialize pins
    pinMode(feedbackPin, INPUT);

    // Attach servo to pin
    servo.attach(servoPin);
    servo.write(90);
    // Read initial angle
    currentAngle = getCurrentAngle();
    targetAngle = currentAngle;

    // Initialize timing
    lastTime = millis();
    Serial.print("lastTime:");
    Serial.println(lastTime);
}

uint8_t CustomServo::getCurrentAngle()
{
    int reading = analogRead(feedbackPin);
    // Map the analog reading to angle (0-180 degrees)
    uint8_t angle = map(reading, calibLow, calibHigh, 0, 180);
    return angle;
}

void CustomServo::update(uint8_t target)
{
    // Enforce a minimum interval between steps to let the servo physically move
    // and for the potentiometer reading to settle before the next nudge.
    unsigned long now = millis();
    if (now - lastTime < STEP_INTERVAL_MS) return;
    lastTime = now;

    Serial.println("Update called");
    // Read actual current angle from potentiometer
    currentAngle = getCurrentAngle();

    int error = (int)target - (int)currentAngle;

    // Already at target - nothing to do
    if (abs(error) <= TOLERANCE) return;

    // Nudge one step in the direction of the target
    int nextAngle = (int)currentAngle + (error > 0 ? STEP_SIZE : -STEP_SIZE);

    // Clamp to valid servo range
    nextAngle = constrain(nextAngle, 0, 180);

    Serial.print("Target: ");
    Serial.print(target);
    Serial.print(" | Current: ");
    Serial.print(currentAngle);
    Serial.print(" | Error: ");
    Serial.print(error);
    Serial.print(" | Writing: ");
    Serial.println(nextAngle);

    servo.write(nextAngle);
}

bool CustomServo::isAtTarget()
{
    return abs((int)targetAngle - (int)currentAngle) <= TOLERANCE;
}