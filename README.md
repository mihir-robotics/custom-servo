# CustomServo Library
07/05/2026: Still very much a WIP!!!

A smooth, step-based servo control library for Arduino. Instead of snappy servo movements, this library uses potentiometer feedback to gradually step toward target angles.

## Features

- **Step-Based Control**: Smooth movement by stepping toward target in fixed increments
- **Feedback-Based**: Reads potentiometer position to ensure accurate servo positioning
- **Auto-Calibration**: Automatically discover potentiometer min/max values
- **1-Degree Tolerance**: Stops when within 1° of target
- **No Overshoot**: Never steps past the target angle

## Hardware Setup

### Required Components
- Arduino Nano (or compatible)
- SG90 servo motor
- 10kΩ potentiometer (connected to servo output shaft)
- Wiring:
  - Servo PWM → Pin 3 (digital PWM-capable pin)
  - Potentiometer → A0 (analog input)
  - Servo GND → Arduino GND
  - Servo 5V → Arduino 5V

## Installation

Copy `CustomServo.h` and `CustomServo.cpp` to your project folder alongside `main.ino`. Include the library in your sketch:

```cpp
#include "CustomServo.h"
```

## Usage

### Basic Setup (with Hardcoded Calibration)

```cpp
#include <Servo.h>
#include "CustomServo.h"

#define CALIBRATION_LOW 78
#define CALIBRATION_HIGH 588

CustomServo serv;
uint8_t servoPin = 3;
uint8_t readPin = A0;

void setup()
{
    Serial.begin(9600);
    serv.begin(servoPin, readPin, CALIBRATION_LOW, CALIBRATION_HIGH);
}

void loop()
{
    // Move to 90 degrees smoothly
    serv.update(90);
    delay(50);  // Update every 50ms for smooth control
}
```

### Auto-Calibration Mode

If you don't know your calibration values, use the automatic calibration:

```cpp
void setup()
{
    Serial.begin(9600);
    
    // Initialize with default 0-1023 range
    serv.begin(servoPin, readPin);
    delay(1000);
    
    // Auto-calibrate: moves to 0° and 180°, reads potentiometer values
    serv.calibrate(2000);  // 2-second delay at each extreme
}
```

**What happens during calibration:**
1. Servo moves to 0° and waits 2 seconds
2. Potentiometer reading recorded as `calibLow`
3. Servo moves to 180° and waits 2 seconds
4. Potentiometer reading recorded as `calibHigh`
5. Servo returns to 90°
6. Calibration values printed to Serial Monitor

Once calibration is complete, you can extract the values and hardcode them for faster startup in the future.

---

## How the CustomServo Library Works

### **Overall Architecture**
The `CustomServo` class wraps the standard Servo library and adds **feedback control** via a potentiometer. Instead of just writing angles directly to the servo, it gradually steps toward the target angle while monitoring the actual position.

---

### **Initialization: `begin()`**
```cpp
serv.begin(servoPin, readPin, CALIBRATION_LOW, CALIBRATION_HIGH);
```
- Stores your pin assignments and calibration values
- Attaches the Servo object to the PWM pin
- Writes initial position (90°) and waits for servo to settle
- Reads the current potentiometer value to know the servo's starting position
- Records `lastTime = millis()` to track timing for step intervals

---

### **Step Control: `update(targetAngle)`**
This is called repeatedly and moves the servo smoothly toward the target:

**Step 1: Check timing**
```cpp
if (now - lastTime < STEP_INTERVAL_MS) return;
```
Ensures minimum time between steps for servo to physically move.

**Step 2: Calculate error**
```cpp
int error = target - currentAngle;
```
If target is 90° and servo is at 45°, error = 45.

**Step 3: Step toward target**
```cpp
if (error > 0)
    commandedAngle = min(commandedAngle + STEP_SIZE, target);
else
    commandedAngle = max(commandedAngle - STEP_SIZE, target);
```
Moves toward target by `STEP_SIZE` degrees, but never overshoots.

**Step 4: Send command**
```cpp
servo.write(commandedAngle);
```
Writes the stepped angle to the servo.

---

### **Reading Feedback: `getCurrentAngle()`**
```cpp
int reading = analogRead(feedbackPin);
uint8_t angle = map(reading, calibLow, calibHigh, 0, 180);
```
Reads the potentiometer analog value (0-1023) and converts it to a 0-180° angle using your calibration values. This is how the library **knows** where the servo actually is.

---

### **Why This Is Better Than Original Code**
Your old code did: `serv.write(pos)` which tells the servo "snap to 45°" instantly, causing jerky motion.

Your new code does: "Step toward 45° gradually" by taking small steps while monitoring position. The servo moves smoothly without overshooting.

---

## API Reference

### Initialization

#### `void begin(uint8_t servoPin, uint8_t feedbackPin, int calibLow = 0, int calibHigh = 1023)`
Initialize the servo with pin assignments and optional calibration values.
- **servoPin**: PWM pin for servo control
- **feedbackPin**: Analog pin for potentiometer feedback
- **calibLow**: Lower calibration value (default: 0)
- **calibHigh**: Upper calibration value (default: 1023)

#### `void calibrate(int delayMs = 2000)`
Automatically calibrate by moving to 0° and 180°.
- **delayMs**: Delay in milliseconds at each extreme for settling (default: 2000ms)

### Control

#### `void update(uint8_t targetAngle)`
Update the servo position by stepping toward the specified target angle. Call this regularly in your loop.
- **targetAngle**: Desired angle (0-180 degrees)

#### `uint8_t getCurrentAngle()`
Read and return the current servo angle from potentiometer feedback.

#### `bool isAtTarget()`
Check if servo is at target angle within 1° tolerance.

### Tuning

#### `void setPIDCoefficients(float kp, float ki, float kd)`
Adjust PID gains for different servo characteristics.

**Default values:**
- `Kp = 1.5` (Proportional: responds to current error)
- `Ki = 0.1` (Integral: eliminates steady-state error)
- `Kd = 0.8` (Derivative: dampens overshoot and oscillation)

**Tuning guide:**
- Increase `Kp` for faster response, decrease if too jerky
- Increase `Kd` if motion is oscillating
- Keep `Ki` small to avoid wind-up

## Example: Smooth Sweep

```cpp
void loop()
{
    // Sweep forward (0 to 180 degrees)
    for (int targetPos = 0; targetPos <= 180; targetPos++)
    {
        serv.update(targetPos);
        delay(50);  // Update every 50ms for smooth control
    }

    // Sweep backward (180 to 0 degrees)
    for (int targetPos = 180; targetPos >= 0; targetPos--)
    {
        serv.update(targetPos);
        delay(50);
    }
}
```

## Troubleshooting

### Servo not moving smoothly
- Decrease `STEP_SIZE` for finer control (try 1, 2, 3 degrees)
- Increase update interval for slower movement

### Servo moving too slowly
- Increase `STEP_SIZE` (try 5, 10 degrees)
- Decrease update interval (try 20-30ms)

### Servo overshoots target
- Decrease `STEP_SIZE`
- The current implementation should never overshoot due to the min/max logic

### Calibration values seem wrong
- Ensure potentiometer is mechanically coupled to servo output
- Check that potentiometer is at full rotation extremes at 0° and 180°
- Run calibration again with increased `delayMs` (try 3000-5000ms)

### Servo not responding at all
- Check power supply (servo needs adequate current)
- Verify wiring connections
- Test with basic `Servo.h` library first
- Ensure calibration values match your hardware

## License

Open source - use freely in your projects!