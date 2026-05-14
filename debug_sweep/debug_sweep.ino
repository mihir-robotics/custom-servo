// Debug script to test servo sweep and potentiometer readings
// Uses only the standard Servo.h library, no CustomServo functionality

#include <Servo.h>

Servo debugServo;
uint8_t servoPin = 3;
uint8_t feedbackPin = A1;
int delayMs = 20;  // Delay between angle steps to let servo settle

void setup()
{
    Serial.begin(9600);
    delay(1000);
    
    pinMode(feedbackPin, INPUT);
    debugServo.attach(servoPin);
}

void loop()
{
    // Sweep forward (0 to 180 degrees)
    for (int angle = 0; angle <= 180; angle += 1)
    {
        debugServo.write(angle);
        delay(delayMs);
        
        int analogValue = analogRead(feedbackPin);
        Serial.print(angle);
        Serial.print("\t");
        // Serial.print("Analog Value:");
        // Serial.print(analogValue);
        // Serial.print(",");
        // Serial.print("Actual Angle:");
        Serial.println(map(analogValue, 62, 555, 0, 180));
    }
    
    delay(100);
    
    // Sweep backward (180 to 0 degrees)
    for (int angle = 180; angle >= 0; angle -= 1)
    {
        debugServo.write(angle);
        delay(delayMs);
        
        int analogValue = analogRead(feedbackPin);
        Serial.print(angle);
        Serial.print("\t");
        // Serial.print("Analog Value:");
        // Serial.print(analogValue);
        // Serial.print(",");
        // Serial.print("Actual Angle:");
        Serial.println(map(analogValue, 62, 555, 0, 180));
    }
    
    delay(100);
}
