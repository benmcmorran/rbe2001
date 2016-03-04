#include "Motion.h"

// Define pins for bumpers and motors
#define FRONT_BUMPER_PIN 25

#define ARM_LEFT_TOP_BUMPER_PIN 27
#define ARM_LEFT_BOTTOM_BUMPER_PIN 29
#define ARM_RIGHT_TOP_BUMPER_PIN 26
#define ARM_RIGHT_BOTTOM_BUMPER_PIN 28

#define ARM_MOTOR_LEFT_PIN 8
#define ARM_MOTOR_RIGHT_PIN 9
#define ARM_MOTOR_INTAKE_PIN 7

/*
 * Constructs a new Motion. initialize() must be called before using
 * the Motion to attach motors and set pin modes.
 */
Motion::Motion() :
  sensor(lineSensorPins),
  leftEnc(19, 20),
  rightEnc(18, 3),
  setpoint(0), input(0), output(0),
  pid(&input, &output, &setpoint, 10, 0, 2, DIRECT) {
}

/*
 * Attaches motors and sets pin modes. This is a separate function
 * because the Arduino libraries must initialize before attaching,
 * so putting this functionality in the constructor would cause it
 * to run too early.
 */
void Motion::initialize() {
  left.attach(10);
  right.attach(11);
  pinMode(FRONT_BUMPER_PIN, INPUT_PULLUP);
  pid.SetOutputLimits(-80, 80);

  pinMode(ARM_LEFT_TOP_BUMPER_PIN, INPUT_PULLUP);
  pinMode(ARM_LEFT_BOTTOM_BUMPER_PIN, INPUT_PULLUP);
  pinMode(ARM_RIGHT_TOP_BUMPER_PIN, INPUT_PULLUP);
  pinMode(ARM_RIGHT_BOTTOM_BUMPER_PIN, INPUT_PULLUP);

  armLeft.attach(ARM_MOTOR_LEFT_PIN);
  armRight.attach(ARM_MOTOR_RIGHT_PIN);
  armIntake.attach(ARM_MOTOR_INTAKE_PIN);
}

/*
 * Returns true if the current state has finished processing.
 */
bool Motion::isDone() {
  return done;
}

/*
 * Starts a 90 degree right turn.
 */
void Motion::turnRight() {
  state = TURN_RIGHT;
  setupEncoderMotion(320, 75, -150, 105);
}

/*
 * Starts a 90 degree left turn.
 */
void Motion::turnLeft() {
  state = TURN_LEFT;
  setupEncoderMotion(-150, 105, 320, 75);
}

/*
 * Spins in place 180 degrees.
 */
void Motion::turn180() {
  state = TURN_180;
  setupEncoderMotion(520, 80, -520, 107);
}

/*
 * Backs up a few inches to get away from the fuel rods.
 */
void Motion::reverse() {
  state = TURN_180;
  setupEncoderMotion(-230, 105, -230, 108);
}

/*
 * Line follows while counting intersections. Stops when count
 * intersections have been crossed.
 */
void Motion::trackToIntersection(int count) {
  state = TRACK_TO_INTERSECTION;
  done = false;
  isInIntersection = false;
  intersectionCount = 0;
  intersectionTarget = count;
}

/*
 * Line follows until the front bumper is pressed. The motors continue
 * to be driven for 1 second after contact to ensure a snug fit.
 */
void Motion::trackToBump() {
  state = TRACK_TO_BUMP;
  done = false;
  hitFrontBump = false;
}

/*
 * Moves the arm to the down position.
 */
void Motion::armDown() {
  state = ARM_DOWN;
  hitLeft = hitRight = false;
  done = false;
}

/*
 * Moves the arm to the up position. The intake is spun out during the
 * motion if dispensingRod is true. This can help avoid pulling the fuel
 * rod back out of the reactor.
 */
void Motion::armUp(bool dispensingRod) {
  state = ARM_UP;
  hitLeft = hitRight = false;
  this->dispensingRod = dispensingRod;
  done = false;
}

/*
 * Spins the intake rollers in for a short while.
 */
void Motion::intakeIn() {
  state = INTAKE_IN;
  intakeStartTime = millis();
  done = false;
}

/*
 * Spins the intake rollers our for a short while.
 */
void Motion::intakeOut() {
  state = INTAKE_OUT;
  intakeStartTime = millis();
  done = false;
}

/*
 * Updates the robot's motion based on its current state and inputs. This
 * function should be called periodically. It is safe to continue calling
 * update after the motion has finished or after calling stop().
 */
void Motion::update() {
  if (done || isStopped) return;

  switch (state) {
    // All encoder states drive the same way
    case TURN_RIGHT:
    case TURN_LEFT:
    case TURN_180:
    case BACKWARDS:
      driveFromEncoder();
      break;
    // Line tracking states drive basically the same, but the end
    // condition is different.
    case TRACK_TO_INTERSECTION:
      if (intersectionCount == intersectionTarget) {
        left.write(90);
        right.write(90);
        done = true;
      } else {
        trackLine();
      }
      break;
    case TRACK_TO_BUMP:
      if (hitFrontBump && millis() - frontBumpTime > 1000) {
        left.write(90);
        right.write(90);
        done = true;
      } else if (!hitFrontBump && digitalRead(FRONT_BUMPER_PIN) == LOW) {
        hitFrontBump = true;
        frontBumpTime = millis();
      } else {
        trackLine();
      }
      break;
    // Arm states drive the same except for pins and direction
    case ARM_DOWN:
      moveArm(ARM_LEFT_BOTTOM_BUMPER_PIN, ARM_RIGHT_BOTTOM_BUMPER_PIN, 83);
      break;
    case ARM_UP:
      moveArm(ARM_LEFT_TOP_BUMPER_PIN, ARM_RIGHT_TOP_BUMPER_PIN, 105);
      if (dispensingRod && !done) armIntake.write(80);
      else armIntake.write(90);
      break;
    // Intake states just drive at different speeds
    case INTAKE_IN:
      driveIntake(110);
      break;
    case INTAKE_OUT:
      driveIntake(80);
      break;
  }
} 

/*
 * Temporarily stops the robot's motion. It can be resumed with resume().
 */
void Motion::stop() {
  isStopped = true;
  left.write(90);
  right.write(90);
  armLeft.write(90);
  armRight.write(90);
  armIntake.write(90);
}

/*
 * Resumes the robot's motion after calling stop().
 */
void Motion::resume() {
  isStopped = false;
}

/*
 * Configures internal state for an encoder motion.
 */
void Motion::setupEncoderMotion(int leftLimit, int leftSpeed, int rightLimit, int rightSpeed) {
  done = false;
  pid.SetMode(MANUAL);
  this->leftLimit = leftLimit;
  this->rightLimit = rightLimit;
  this->leftSpeed = leftSpeed;
  this->rightSpeed = rightSpeed;
  resetEncoders();
}

/*
 * Sets both encoders back to zero.
 */
void Motion::resetEncoders() {
  leftEnc.write(0);
  rightEnc.write(0);
}

/*
 * Drives the wheels based on values set by setupEncoderMotion(). done
 * is true when both left and right have met their target.
 */
void Motion::driveFromEncoder() {
  // Check if we have met each target, accounting for the direction of
  // the desired rotation
  bool driveLeft = leftLimit < 0 ? leftEnc.read() >= leftLimit : leftEnc.read() <= leftLimit;
  bool driveRight = rightLimit < 0 ? rightEnc.read() >= rightLimit : rightEnc.read() <= rightLimit;

  // Spin the motor at the desired speed if there's still distance left
  left.write(driveLeft ? leftSpeed : 90);
  right.write(driveRight ? rightSpeed : 90);

  // Check if we're done yet
  done = !driveLeft & !driveRight;
}

/*
 * Slowly drives the robot forward while following a line.
 */
void Motion::trackLine() {
  // Get new sensor values
  sensor.update();
  int averageReading = sensor.averageReading();
  
  // Use hysteresis to count intersections
  // Only count on the falling edge
  if (isInIntersection && averageReading < negIntThresh) {
    isInIntersection = false;
  } else if (!isInIntersection && averageReading > posIntThresh) {
    isInIntersection = true;
    intersectionCount++;
  }
  
  // Only follow when a line is detected
  if (sensor.isLineDetected()) {
    // The centroid should be valid if a line is detected
    input = sensor.averageLinePosition();
    // Use a PID loop to compute motor inputs
    pid.SetMode(AUTOMATIC);
    pid.Compute();
    // Use a bias of 83 to provide slow forward motion
    left.write(83 + output);
    right.write(83 - output);
  // Otherwise continue with the previous motor inputs
  } else {
    pid.SetMode(MANUAL);
  }
}

/*
 * Moves the arm motors at the desired speed until both bumpers
 * have triggered. Each side is driven independently and continues
 * to drive for 200ms after contact to ensure a snug fit.
 */
void Motion::moveArm(int leftBumpPin, int rightBumpPin, int speed) {
  long now = millis();
  bool leftDone, rightDone;
  leftDone = rightDone = false;
  
  // Check if we hit
  if (!hitLeft && digitalRead(leftBumpPin) == LOW) {
    hitLeft = true;
    leftHitTime = millis();
  // Check if we're done with overdrive
  } else if (hitLeft && now - leftHitTime > 200) {
    armLeft.write(90);
    leftDone = true;
  // Otherwise move normally
  } else {
    armLeft.write(speed);
  }

  if (!hitRight && digitalRead(rightBumpPin) == LOW) {
    hitRight = true;
    rightHitTime = millis();
  } else if (hitRight && now - rightHitTime > 200) {
    armRight.write(90);
    rightDone = true;
  } else {
    armRight.write(speed);
  }

  // Check if we're done
  done = leftDone && rightDone;
}

/*
 * Spins the intake motor at the desired speed for 2 seconds.
 */
void Motion::driveIntake(int speed) {
  if (millis() - intakeStartTime > 2000) {
    armIntake.write(90);
    done = true;
  } else {
    armIntake.write(speed);
  }
}

