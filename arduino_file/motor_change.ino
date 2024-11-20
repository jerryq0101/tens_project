#include <Servo.h>

Servo myServo;
const int SERVO_PIN = 9;
const int MAX_ANGLE = 170;  // Maximum safe angle (the servo starts tweaking at 160)
const int MIN_ANGLE = 0;    // Minimum angle
const int MOVE_STEP = 10;   // Degrees to move per step
const int MOVE_DELAY = 30;  // Delay between movements

int currentAngle = MAX_ANGLE;       // Current servo position
bool isMoving = false;      // Whether servo is currently in motion
bool isIncreasing = false;  // Direction of movement

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  myServo.write(currentAngle);  // Initialize position (at 150, so that we can turn the servo via tens clockwise)
  delay(MOVE_DELAY);
  // Serial.println("TENS Unit Control Ready");
  // Serial.println("Enter 1 to start, 0 to stop");

}

void loop() {
  // Handle serial input if available
  if (Serial.available() > 0) {
    processSerialInput();
  }

  // Handle servo movement if active
  if (isMoving) {
    moveServo();
  }

  delay(MOVE_DELAY);  // Consistent timing
}

void processSerialInput() {
  int input = Serial.read() - '0';
  
  if (input == 1) {  // Start or continue movement
    Serial.println("TENS unit ON");
    isMoving = true;
    isIncreasing = false;
  }
  else if (input == 0) {  // Stop or reverse movement
    Serial.println("TENS unit OFF");
    if (currentAngle <= MIN_ANGLE) {  // Only if at the minimum, increase back to the maximum angle
      isMoving = true;
      isIncreasing = true;
    }
  }
}

void moveServo() {

  if (isIncreasing) {
    // Moving up
    if (currentAngle < MAX_ANGLE) {
      currentAngle = min(currentAngle + MOVE_STEP, MAX_ANGLE);
      myServo.write(currentAngle);
      // Serial.print("Intensity increasing: ");
      // Serial.println(currentAngle);
    } else {
      // Reached maximum
      isMoving = false;
      // Serial.println("Maximum intensity reached");
    }
  } else // !isIncreasing

  {
    // Moving down
    if (currentAngle > MIN_ANGLE) {
      currentAngle = max(currentAngle - MOVE_STEP, MIN_ANGLE);
      myServo.write(currentAngle);
      // Serial.print("Intensity decreasing: ");
      // Serial.println(currentAngle);
    } else {
      // Reached minimum
      isMoving = false;
      // Serial.println("TENS unit fully off");
    }
  }

}
