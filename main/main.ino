// Code to control customised SG90 Servo with smooth PID-based movement
#include <Servo.h>
#include <CustomServo.h>

// Defining the lowest and highest voltage values for potentiometer calibration
#define CALIBRATION_LOW 309
#define CALIBRATION_HIGH 646

// CustomServo object for PID-controlled feedback
CustomServo serv;

// Declaring read and write pins for servo on board
uint8_t servoPin = 3;
uint8_t readPin = 26;  // pin 26 is A0 on Pico

// Update timing
unsigned long lastUpdateTime = 0;
int UPDATE_INTERVAL_MS = 10;  // Update PID control every 10ms for smooth movement

//  Setting up the connections
void setup()
{
    Serial.begin(9600);
    
    // Initialize CustomServo with calibration values
    serv.begin(servoPin, readPin, CALIBRATION_LOW, CALIBRATION_HIGH);
    
    // Optional: Tune PID coefficients (default: Kp=1.5, Ki=0.1, Kd=0.8)
    // serv.setPIDCoefficients(1.5, 0.1, 0.8);
}

// Main loop with smooth servo movement
void loop()
{
    // Sweep forward (0 to 180 degrees)
    for (int targetPos = 0; targetPos <= 180; targetPos++)
    {
        serv.setTargetAngle(targetPos);
        
        // Keep updating PID control until we reach target
        while (!serv.isAtTarget())
        {
            serv.update();
            
            // Print current angle for monitoring
            uint8_t currentAngle = serv.getCurrentAngle();
            Serial.print("Target: ");
            Serial.print(targetPos);
            Serial.print(" | Current: ");
            Serial.println(currentAngle);
            
            delay(UPDATE_INTERVAL_MS);
        }
    }

    // Sweep backward (180 to 0 degrees)
    for (int targetPos = 180; targetPos >= 0; targetPos--)
    {
        serv.setTargetAngle(targetPos);
        
        // Keep updating PID control until we reach target
        while (!serv.isAtTarget())
        {
            serv.update();
            
            // Print current angle for monitoring
            uint8_t currentAngle = serv.getCurrentAngle();
            Serial.print("Target: ");
            Serial.print(targetPos);
            Serial.print(" | Current: ");
            Serial.println(currentAngle);
            
            delay(UPDATE_INTERVAL_MS);
        }
    }
}