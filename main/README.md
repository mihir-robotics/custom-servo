## How the CustomServo Library Works

### **Overall Architecture**
The `CustomServo` class wraps the standard Servo library and adds **feedback control** via a potentiometer. Instead of just writing angles directly to the servo, it continuously reads where the servo actually is (via potentiometer) and uses PID math to smoothly move it toward your target.

---

### **Initialization: `begin()`**
```cpp
serv.begin(servoPin, readPin, CALIBRATION_LOW, CALIBRATION_HIGH);
```
- Stores your pin assignments and calibration values
- Attaches the Servo object to the PWM pin
- Reads the current potentiometer value to know the servo's starting position
- Sets `targetAngle = currentAngle` (no movement until you command it)
- Records `lastTime = millis()` to track timing for the PID algorithm

---

### **Setting a Target: `setTargetAngle()`**
```cpp
serv.setTargetAngle(90);  // Command: move to 90 degrees
```
Simply stores your desired angle. The actual movement happens in `update()`.

---

### **The Heart: PID Control Loop in `update()`**
This is called repeatedly (every 10ms in your main loop) and does the smooth movement magic:

**Step 1: Calculate error**
```cpp
int error = targetAngle - currentAngle;
```
If target is 90° and servo is at 45°, error = 45.

**Step 2: PID calculation** (three components combined):

| Component | Formula | Purpose |
|-----------|---------|---------|
| **P (Proportional)** | `Kp * error` | React immediately to how far off you are |
| **I (Integral)** | `Ki * (accumulated error over time)` | Eliminate steady-state error; keeps pushing if stuck |
| **D (Derivative)** | `Kd * (change in error)` | Dampen oscillations; smooth out overshoot |

```cpp
float P = Kp * error;                          // React to current error
integral += error * dt;                        // Build up over time
float I = Ki * integral;                       // Apply accumulated effort
float D = Kd * (error - lastError) / dt;       // React to rate of change
float output = P + I + D;                      // Combined control signal
```

**Step 3: Convert to servo angle and write**
```cpp
int servoAngle = currentAngle + (int)output;   // Current position + adjustment
servoAngle = constrainServoValue(servoAngle);  // Keep within 0-180
servo.write(servoAngle);                       // Send command
```

---

### **Reading Feedback: `readFeedback()`**
```cpp
int reading = analogRead(feedbackPin);
uint8_t angle = map(reading, calibLow, calibHigh, 0, 180);
```
Reads the potentiometer analog value (0-1023) and converts it to a 0-180° angle using your calibration values. This is how the library **knows** where the servo actually is.

---

### **Stopping Condition: `isAtTarget()`**
```cpp
bool isAtTarget() {
    return abs((int)targetAngle - (int)currentAngle) <= TOLERANCE;  // TOLERANCE = 1 degree
}
```
Returns `true` when you're within 1° of target. The main loop waits for this before moving to the next angle.

---

### **Main Loop Flow**
```cpp
for (int targetPos = 0; targetPos <= 180; targetPos++) {
    serv.setTargetAngle(targetPos);           // Command new target
    
    while (!serv.isAtTarget()) {              // Loop until reached
        serv.update();                        // PID adjusts servo
        delay(UPDATE_INTERVAL_MS);            // Wait 10ms, repeat
    }
}
```

**Example: Moving 0° → 1°:**
1. Set target to 1°
2. Call `update()` → servo gets small adjustment toward 1°
3. Read potentiometer → still at 0.5°
4. PID calculates: error=0.5°, adjust servo slightly
5. Repeat every 10ms until within 1° of 1° (i.e., 0° to 2°)
6. Exit loop, move to next target

---

### **Why This Is Better Than Original Code**
Your old code did: `serv.write(pos)` which tells the servo "snap to 45°" instantly, causing jerky motion.

Your new code does: "Move toward 45° smoothly using feedback" by continuously adjusting based on where it actually is. The servo accelerates, cruises, then decelerates smoothly.