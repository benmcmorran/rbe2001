#include "Motion.h"
#include "Navigator.h"
#include <Encoder.h>
#include <Servo.h>
#include <PID_v1.h>
#include <TimerOne.h>

Motion motion;
Navigator navigator(&motion);
char commandBuffer[10];

void setup() {
  motion.initialize();
  motion.armUp(); wait();
  motion.trackToBump(); wait();
  motion.armDown(); wait();
  motion.intakeIn(); wait();
  motion.armUp(); wait();
  
  goTo(SPENT_ROD_2);

  motion.intakeOut(); wait();
  
  goTo(NEW_ROD_3);

  motion.intakeIn(); wait();
  
  goTo(REACTOR_A);

  motion.armDown(); wait();
  motion.intakeOut(); wait();
  motion.armUp(); wait();
  
  goTo(REACTOR_B);

  motion.armDown(); wait();
  motion.intakeIn(); wait();
  motion.armUp(); wait();
  
  goTo(SPENT_ROD_3);

  motion.intakeOut(); wait();
  
  goTo(NEW_ROD_4);

  motion.intakeIn(); wait();
  
  goTo(REACTOR_B);

  motion.armDown(); wait();
  motion.intakeOut(); wait();
  motion.armUp(); wait();
}

void goTo(enum NavigatorPosition pos) {
  int length = navigator.buildPlan(pos, commandBuffer);
  navigator.executePlan(commandBuffer, length);

  while (!navigator.isDone()) {
    navigator.update();
  }
}

void wait() {
  while (!motion.isDone()) {
    motion.update();
  }
}

void loop() {
}

