#ifndef MOTION_H
#define MOTION_H

#include <Arduino.h>
#include <Servo.h>
#include <Encoder.h>
#include <PID_v1.h>

#include "LineSensor.h"

/*
 * MotionState represents all the possible actions the robot can take.
 * The states are explictly numbered so that they can be packed into
 * a char by the Navigator.
 */
enum MotionState {
  TURN_RIGHT = 0,
  TURN_LEFT = 1,
  TURN_180 = 2,
  BACKWARDS = 3,
  TRACK_TO_INTERSECTION = 4,
  TRACK_TO_BUMP = 5,
  ARM_DOWN = 6,
  ARM_UP = 7,
  INTAKE_IN = 8,
  INTAKE_OUT = 9
};

/*
 * Motion drives all motion on the robot. It is designed to be non-blocking
 * by simply calling the appropriate function and then repeatedly calling
 * update().
 */
class Motion {
public:
  Motion();
  void initialize();
  bool isDone();
  void turnRight();
  void turnLeft();
  void turn180();
  void reverse();
  void trackToIntersection(int count);
  void trackToBump();
  void armDown();
  void armUp(bool dispensingRod);
  void intakeIn();
  void intakeOut();
  void update();
  void stop();
  void resume();

private:
  void setupEncoderMotion(int leftLimit, int leftSpeed, int rightLimit, int rightSpeed);
  void resetEncoders();
  void driveFromEncoder();
  void trackLine();
  void moveArm(int leftBumpPin, int rightBumpPin, int speed);
  void driveIntake(int speed);

  const int lineSensorPins[8] = { 5, 6, 7, 1, 0, 2, 3, 4 };
  LineSensor sensor;

  const int posIntThresh = 300;
  const int negIntThresh = 200;
  bool isInIntersection = false;
  int intersectionCount = 0;
  int intersectionTarget = 0;

  Servo left, right;
  Encoder leftEnc, rightEnc;
  int leftLimit, leftSpeed, rightLimit, rightSpeed;

  double setpoint, input, output;
  PID pid;
  bool hitFrontBump;
  long frontBumpTime;

  Servo armLeft, armRight, armIntake;
  bool hitLeft, hitRight;
  long leftHitTime, rightHitTime, intakeStartTime;
  bool dispensingRod;

  bool isStopped = false;
  bool done = true;
  enum MotionState state;
};

#endif

