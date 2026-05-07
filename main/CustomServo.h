/**
 * Custom Servo Library for Modified MG90S Motor
 * 
 * Provides feedback-based servo control using incremental step movement
 * for smooth, non-jerky positioning. A potentiometer wired to the servo
 * shaft provides real angle feedback on every update() call.
 */

#ifndef CUSTOM_SERVO_H
#define CUSTOM_SERVO_H

#include "Arduino.h"
#include "Servo.h"

class CustomServo
{
public:
    // Constructor
    CustomServo();

    // Destructor
    ~CustomServo();

    /**
     * Initialize the servo with pin assignments and calibration values.
     * @param servoPin    PWM pin for servo control
     * @param feedbackPin Analog pin for potentiometer feedback
     * @param calibLow    Potentiometer ADC reading at 0 degrees
     * @param calibHigh   Potentiometer ADC reading at 180 degrees
     */
    void begin(uint8_t servoPin, uint8_t feedbackPin, int calibLow, int calibHigh);

    /**
     * Read and return current servo angle from potentiometer feedback.
     * @return Current angle in degrees (0-180)
     */
    uint8_t getCurrentAngle();

    /**
     * Step the servo one degree closer to target. Call this repeatedly in
     * loop() — each call moves the servo by one step (STEP_SIZE degrees)
     * and returns immediately; the rate of movement is governed by
     * STEP_INTERVAL_MS. Uses potentiometer feedback to read true position
     * before each step.
     * @param target Desired angle (0-180 degrees)
     */
    void update(uint8_t target);

    /**
     * Check if the servo has reached its target angle within tolerance.
     * @return true if within TOLERANCE degrees of target
     */
    bool isAtTarget();

private:
    Servo servo;
    uint8_t servoPin;
    uint8_t feedbackPin;

    // Calibration values for potentiometer-to-angle mapping
    int calibLow;
    int calibHigh;

    // Angle state
    uint8_t targetAngle;
    uint8_t currentAngle;
    uint8_t commandedAngle;  // Last angle written to servo — stepped monotonically

    // Timing
    unsigned long lastTime;

    // How many ms to wait between steps. Controls movement speed.
    // Lower = faster but risks the pot reading not settling in time.
    static const uint8_t STEP_INTERVAL_MS = 10;

    // Degrees of acceptable error at target position
    static const uint8_t TOLERANCE = 5;
};

#endif
