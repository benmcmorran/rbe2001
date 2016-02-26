#include "Motion.h"
#include <Encoder.h>
#include <Servo.h>
#include <PID_v1.h>
#include <TimerOne.h>

Motion motion;

void setup() {
  motion.initialize();
}

void loop() {
  motion.armUp();
  while (!motion.isDone()) motion.update();
  motion.intakeIn();
  while (!motion.isDone()) motion.update();
  motion.armDown();
  while (!motion.isDone()) motion.update();
  motion.intakeOut();
  while (!motion.isDone()) motion.update();
  
//  motion.trackToIntersection(2);
//  while (!motion.isDone()) motion.update();
//  motion.turnRight();
//  while (!motion.isDone()) motion.update();
//  motion.trackToBump();
//  while (!motion.isDone()) motion.update();
//  motion.reverse();
//  while (!motion.isDone()) motion.update();
//  motion.turn180();
//  while (!motion.isDone()) motion.update();
//  motion.trackToIntersection(1);
//  while (!motion.isDone()) motion.update();
//  motion.turnRight();
//  while (!motion.isDone()) motion.update();
//  motion.trackToBump();
//  while (!motion.isDone()) motion.update();
//  motion.reverse();
//  while (!motion.isDone()) motion.update();
//  motion.turn180();
//  while (!motion.isDone()) motion.update();
}

