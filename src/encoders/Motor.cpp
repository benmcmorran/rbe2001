#include "Motor.h"

#include <TimerOne.h>

#define MOTOR_MAX_MOTORS 10

static Motor *globalMotors[MOTOR_MAX_MOTORS];
static int globalMotorCount = 0;

void encoderVelocityISR() {
  for (int i = 0; i < globalMotorCount; i++) {
    globalMotors[i]->updateEncoder();
  }
}

void Motor::initialize() {
  Timer1.initialize(50000); // 50000 us = 20 Hz
  Timer1.attachInterrupt(encoderVelocityISR);
}

Motor::Motor(int encoderPin1, int encoderPin2) :
    input(0), output(0), setpoint(0), ticksSampled(0),
    encoder(encoderPin1, encoderPin2),
    pid(&input, &output, &setpoint, 20, 10, .2, DIRECT) {

  pid.SetOutputLimits(-80, 80);
  pid.SetSampleTime(50); // 50 ms = 20 Hz

  motorIndex = globalMotorCount;
  if (globalMotorCount < MOTOR_MAX_MOTORS) {
    globalMotors[globalMotorCount] = this;
    globalMotorCount++;
  }
}

void Motor::attach(int motorPin) {
  motor.attach(motorPin);
}

void Motor::updateEncoder() {
  if (isSpeedMode) {
    ticksSampled = encoder.read();
    encoder.write(0);
  }
}

void Motor::update() {
  if (isSpeedMode) {
    input = (double)ticksSampled * 20 / 360; // sampling at 20 Hz, 360 quad ticks per rev
    pid.Compute();
    motor.write(90 - output);
  } else {
    if (abs(encoder.read()) >= abs(targetTicks))
      motor.write(90);
    else
      motor.write(targetTicks > 0 ? 80 : 105);
  }
}

void Motor::setSpeed(double speed) {
  setpoint = speed;
  pid.SetMode(AUTOMATIC);
  isSpeedMode = true;
}

void Motor::setRotations(double rotations) {
  isSpeedMode = false;
  pid.SetMode(MANUAL);
  encoder.write(0);
  targetTicks = (int)(rotations * 360);
}

bool Motor::isAtTarget() {
  return abs(encoder.read()) >= abs(targetTicks);
}

