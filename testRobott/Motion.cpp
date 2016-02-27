#include "Motion.h"

#define FRONT_BUMPER_PIN 25

#define ARM_LEFT_TOP_BUMPER_PIN 27
#define ARM_LEFT_BOTTOM_BUMPER_PIN 29
#define ARM_RIGHT_TOP_BUMPER_PIN 26
#define ARM_RIGHT_BOTTOM_BUMPER_PIN 28

#define ARM_MOTOR_LEFT_PIN 8
#define ARM_MOTOR_RIGHT_PIN 9
#define ARM_MOTOR_INTAKE_PIN 7

Motion::Motion() :
  sensor(lineSensorPins),
  leftEnc(19, 20),
  rightEnc(18, 3),
  setpoint(0), input(0), output(0),
  pid(&input, &output, &setpoint, 10, 0, 2, DIRECT) {
}

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

bool Motion::isDone() {
  return done;
}

void Motion::turnRight() {
  state = TURN_RIGHT;
  setupEncoderMotion(320, 75, -150, 105);
}

void Motion::turnLeft() {
  state = TURN_LEFT;
  setupEncoderMotion(-150, 105, 320, 75);
}

void Motion::turn180() {
  state = TURN_180;
  setupEncoderMotion(520, 80, -520, 107);
}

void Motion::reverse() {
  state = TURN_180;
  setupEncoderMotion(-230, 105, -230, 108);
}

void Motion::trackToIntersection(int count) {
  state = TRACK_TO_INTERSECTION;
  done = false;
  isInIntersection = false;
  intersectionCount = 0;
  intersectionTarget = count;
}

void Motion::trackToBump() {
  state = TRACK_TO_BUMP;
  done = false;
  hitFrontBump = false;
}

void Motion::armDown() {
  state = ARM_DOWN;
  hitLeft = hitRight = false;
  done = false;
}

void Motion::armUp() {
  state = ARM_UP;
  hitLeft = hitRight = false;
  done = false;
}

void Motion::intakeIn() {
  state = INTAKE_IN;
  intakeStartTime = millis();
  done = false;
}

void Motion::intakeOut() {
  state = INTAKE_OUT;
  intakeStartTime = millis();
  done = false;
}

void Motion::update() {
  if (done) return;

  switch (state) {
    case TURN_RIGHT:
    case TURN_LEFT:
    case TURN_180:
    case BACKWARDS:
      driveFromEncoder();
      break;
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
    case ARM_DOWN:
      moveArm(ARM_LEFT_BOTTOM_BUMPER_PIN, ARM_RIGHT_BOTTOM_BUMPER_PIN, 83);
      break;
    case ARM_UP:
      moveArm(ARM_LEFT_TOP_BUMPER_PIN, ARM_RIGHT_TOP_BUMPER_PIN, 105);
      break;
    case INTAKE_IN:
      driveIntake(110);
      break;
    case INTAKE_OUT:
      driveIntake(80);
      break;
  }
} 

void Motion::setupEncoderMotion(int leftLimit, int leftSpeed, int rightLimit, int rightSpeed) {
  done = false;
  pid.SetMode(MANUAL);
  this->leftLimit = leftLimit;
  this->rightLimit = rightLimit;
  this->leftSpeed = leftSpeed;
  this->rightSpeed = rightSpeed;
  resetEncoders();
}

void Motion::resetEncoders() {
  leftEnc.write(0);
  rightEnc.write(0);
}

void Motion::driveFromEncoder() {
  bool driveLeft = leftLimit < 0 ? leftEnc.read() >= leftLimit : leftEnc.read() <= leftLimit;
  bool driveRight = rightLimit < 0 ? rightEnc.read() >= rightLimit : rightEnc.read() <= rightLimit;
  left.write(driveLeft ? leftSpeed : 90);
  right.write(driveRight ? rightSpeed : 90);
  done = !driveLeft & !driveRight;
}

void Motion::trackLine() {
  sensor.update();
  int averageReading = sensor.averageReading();
  
  if (isInIntersection && averageReading < negIntThresh) {
    isInIntersection = false;
  } else if (!isInIntersection && averageReading > posIntThresh) {
    isInIntersection = true;
    intersectionCount++;
  }
  
  if (sensor.isLineDetected()) {
    input = sensor.averageLinePosition();
    pid.SetMode(AUTOMATIC);
    pid.Compute();
    left.write(83 + output);
    right.write(83 - output);
  } else {
//    left.write(80);
//    right.write(80);
    pid.SetMode(MANUAL);
  }
}

void Motion::moveArm(int leftBumpPin, int rightBumpPin, int speed) {
  long now = millis();
  bool leftDone, rightDone;
  leftDone = rightDone = false;
  
  if (!hitLeft && digitalRead(leftBumpPin) == LOW) {
    hitLeft = true;
    leftHitTime = millis();
  } else if (hitLeft && now - leftHitTime > 200) {
    armLeft.write(90);
    leftDone = true;
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

  done = leftDone && rightDone;
}

void Motion::driveIntake(int speed) {
  if (millis() - intakeStartTime > 2000) {
    armIntake.write(90);
    done = true;
  } else {
    armIntake.write(speed);
  }
}

