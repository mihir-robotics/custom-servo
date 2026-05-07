/**
 * Custom Servo Library for MG90S Motor
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
     * Initialize the servo with pin assignments and calibration values
     * @param servoPin PWM pin for servo control
     * @param feedbackPin Analog pin for potentiometer feedback
     * @param calibLow Lower calibration value from potentiometer (default: 0)
     * @param calibHigh Upper calibration value from potentiometer (default: 1023)
     */
    void begin(uint8_t servoPin, uint8_t feedbackPin, int calibLow, int calibHigh);

    /**
     * Read and return current servo angle from potentiometer feedback
     * @return Current angle (0-180 degrees)
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
     * Check if servo is at target angle within tolerance
     * @return true if within 1 degree of target
     */
    bool isAtTarget();

private:
    // Hardware objects and pins
    Servo servo;
    uint8_t servoPin;
    uint8_t feedbackPin;
    
    // Calibration values
    int calibLow;
    int calibHigh;
    uint8_t targetAngle;
    uint8_t currentAngle;

    // Timing
    unsigned long lastTime;

    // How many ms to wait between steps. Controls movement speed.
    // Lower = faster but risks the pot reading not settling in time.
    static const uint8_t STEP_INTERVAL_MS = 20;

    // Degrees of acceptable error at target position
    static const uint8_t TOLERANCE = 5;
};

#endif