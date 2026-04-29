/**
 * Custom Servo Library Implementation with PID Control
 */

#include "Arduino.h"
#include "CustomServo.h"

// Default PID coefficients (can be tuned)
#define DEFAULT_KP 1.5f
#define DEFAULT_KI 0.1f
#define DEFAULT_KD 0.8f

CustomServo::CustomServo()
    : servoPin(0), feedbackPin(0), calibLow(0), calibHigh(1023),
      Kp(DEFAULT_KP), Ki(DEFAULT_KI), Kd(DEFAULT_KD),
      targetAngle(90), currentAngle(90), integral(0), lastError(0), lastTime(0)
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

    // Read initial angle
    currentAngle = readFeedback();
    targetAngle = currentAngle;

    // Initialize timing
    lastTime = millis();
}

void CustomServo::setTargetAngle(uint8_t angle)
{
    // Constrain angle to valid range
    targetAngle = constrain(angle, 0, 180);
}

uint8_t CustomServo::getCurrentAngle()
{
    currentAngle = readFeedback();
    return currentAngle;
}

void CustomServo::update()
{
    // Get current time for dt calculation
    unsigned long currentTime = millis();
    unsigned long dt = currentTime - lastTime;
    lastTime = currentTime;

    // Avoid division by zero
    if (dt == 0) return;

    // Read current angle
    currentAngle = readFeedback();

    // Calculate error
    int error = targetAngle - currentAngle;

    // Stop PID control if at target (within tolerance)
    if (abs(error) <= TOLERANCE)
    {
        integral = 0;  // Reset integral to prevent windup
        lastError = 0;
        return;
    }

    // Proportional term
    float P = Kp * error;

    // Integral term (with anti-windup)
    integral += error * dt;
    integral = constrain(integral, -1000, 1000);  // Limit integral
    float I = Ki * integral;

    // Derivative term
    float D = 0;
    if (dt > 0)
    {
        D = Kd * (error - lastError) / dt;
    }
    lastError = error;

    // Calculate PID output
    float output = P + I + D;

    // Map PID output to servo angle
    int servoAngle = currentAngle + (int)output;
    servoAngle = constrainServoValue(servoAngle);

    // Write to servo
    servo.write(servoAngle);
}

bool CustomServo::isAtTarget()
{
    return abs((int)targetAngle - (int)currentAngle) <= TOLERANCE;
}

void CustomServo::setPIDCoefficients(float kp, float ki, float kd)
{
    Kp = kp;
    Ki = ki;
    Kd = kd;
}

uint8_t CustomServo::readFeedback()
{
    int reading = analogRead(feedbackPin);
    // Map the analog reading to angle (0-180 degrees)
    uint8_t angle = map(reading, calibLow, calibHigh, 0, 180);
    return angle;
}

uint8_t CustomServo::constrainServoValue(int value)
{
    return constrain(value, 0, 180);
}
