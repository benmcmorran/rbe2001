#include "Motor.h"

#include <Encoder.h>
#include <Servo.h>
#include <PID_v1.h>
#include <TimerOne.h>

Motor left(19, 20);
Motor right(18, 3);

void setup() {
  Motor::initialize();
  left.attach(10);
  right.attach(11);
  left.setRotations(0.72);
  right.setRotations(-0.72);
}

void loop() {
  left.update();
  right.update();
}

