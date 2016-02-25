#include "Motion.h"

#define FRONT_BUMPER_PIN 25

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
      if (digitalRead(FRONT_BUMPER_PIN) == LOW) {
        left.write(90);
        right.write(90);
        done = true;
      } else {
        trackLine();
      }
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

