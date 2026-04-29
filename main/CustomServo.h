/**
 * Custom Servo Library for SG90 Motor with PID Control
 * 
 * Provides feedback-based servo control using PID algorithm
 * for smooth, non-jerky movement to target angles.
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
     * @param calibLow Lower calibration value from potentiometer
     * @param calibHigh Upper calibration value from potentiometer
     */
    void begin(uint8_t servoPin, uint8_t feedbackPin, int calibLow, int calibHigh);

    /**
     * Set the target angle for the servo
     * @param angle Desired angle (0-180 degrees)
     */
    void setTargetAngle(uint8_t angle);

    /**
     * Read and return current servo angle from potentiometer feedback
     * @return Current angle (0-180 degrees)
     */
    uint8_t getCurrentAngle();

    /**
     * Update the servo position using PID control
     * Must be called regularly (ideally in a loop with consistent timing)
     */
    void update();

    /**
     * Check if servo is at target angle within tolerance
     * @return true if within 1 degree of target
     */
    bool isAtTarget();

    /**
     * Set PID coefficients for tuning
     * @param kp Proportional gain
     * @param ki Integral gain
     * @param kd Derivative gain
     */
    void setPIDCoefficients(float kp, float ki, float kd);

private:
    // Hardware objects and pins
    Servo servo;
    uint8_t servoPin;
    uint8_t feedbackPin;
    
    // Calibration values
    int calibLow;
    int calibHigh;

    // PID control parameters
    float Kp;  // Proportional gain
    float Ki;  // Integral gain
    float Kd;  // Derivative gain

    // PID state variables
    uint8_t targetAngle;
    uint8_t currentAngle;
    float integral;          // Accumulated integral error
    int lastError;           // Previous error for derivative calculation
    unsigned long lastTime;  // Last update time for dt calculation
    static const uint8_t TOLERANCE = 1;  // 1 degree tolerance

    /**
     * Read analog input and convert to angle
     * @return Mapped angle value (0-180)
     */
    uint8_t readFeedback();

    /**
     * Constrain PWM value for servo
     * @param value PWM value to constrain
     * @return Constrained value (0-180 degrees)
     */
    uint8_t constrainServoValue(int value);
};

#endif
