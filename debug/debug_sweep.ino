// Debug script to test servo sweep and potentiometer readings
// Uses only the standard Servo.h library, no CustomServo functionality

#include <Servo.h>

Servo debugServo;
uint8_t servoPin = 3;
uint8_t feedbackPin = A0;
int delayMs = 1000;  // Delay between angle steps to let servo settle

void setup()
{
    Serial.begin(9600);
    delay(1000);
    
    pinMode(feedbackPin, INPUT);
    debugServo.attach(servoPin);
    
    Serial.println("=== CustomServo Debug Sweep ===");
    Serial.println("Sweeping servo 0 -> 180 -> 0");
    Serial.println("Angle, AnalogValue");
    Serial.println("---");
}

void loop()
{
    // Sweep forward (0 to 180 degrees)
    Serial.println("FORWARD SWEEP:");
    for (int angle = 0; angle <= 180; angle += 10)
    {
        debugServo.write(angle);
        delay(delayMs);
        
        int analogValue = analogRead(feedbackPin);
        Serial.print(angle);
        Serial.print(",");
        Serial.print(analogValue);
        Serial.print(",");
        Serial.print("Mapped:");
        Serial.println(map(analogValue, 70, 575, 0, 180));
    }
    
    delay(1000);
    
    // Sweep backward (180 to 0 degrees)
    Serial.println("BACKWARD SWEEP:");
    for (int angle = 180; angle >= 0; angle -= 10)
    {
        debugServo.write(angle);
        delay(delayMs);
        
        int analogValue = analogRead(feedbackPin);
        Serial.print(angle);
        Serial.print(",");
        Serial.print(analogValue);
        Serial.print(",");
        Serial.print("Mapped:");
        Serial.println(map(analogValue, 70, 575, 0, 180));
    }
    
    delay(2000);
}
