#include "Motion.h"

#define FRONT_BUMPER_PIN 25

Motion::Motion() :
  sensor(lineSensorPins),
  leftEnc(19, 20),
  rightEnc(18, 3),
  setpoint(0), input(0), output(0),
  pid(&input, &output, &setpoint, 20, 0, 3, DIRECT) {
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
  done = false;
  pid.SetMode(MANUAL);
  leftLimit = 320;
  rightLimit = -150;
  leftSpeed = 75;
  rightSpeed = 105;
  resetEncoders();
}

void Motion::turnLeft() {
  state = TURN_LEFT;
  done = false;
  pid.SetMode(MANUAL);
  leftLimit = -150;
  rightLimit = 320;
  leftSpeed = 105;
  rightSpeed = 75;
  resetEncoders();
}

void Motion::turn180() {
  state = TURN_180;
  done = false;
  pid.SetMode(MANUAL);
  leftLimit = 520;
  rightLimit = -520;
  leftSpeed = 80;
  rightSpeed = 107;
  resetEncoders();
}

void Motion::reverse() {
  state = TURN_180;
  done = false;
  pid.SetMode(MANUAL);
  leftLimit = 520;
  rightLimit = -520;
  leftSpeed = 80;
  rightSpeed = 107;
  resetEncoders();
}

void Motion::trackToIntersection(int count) {
  
}

void Motion::trackToBump() {
  
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
    left.write(80 + output);
    right.write(80 - output);
  } else {
    left.write(80);
    right.write(80);
    pid.SetMode(MANUAL);
  }
}

