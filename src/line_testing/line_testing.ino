#include "Motion.h"
#include "Navigator.h"
#include <Encoder.h>
#include <Servo.h>
#include <PID_v1.h>
#include <TimerOne.h>

Motion motion;
Navigator navigator(&motion);

void setup() {
  motion.initialize();
  Serial.begin(9600);
  delay(3000);
  Serial.println("Starting pathing");
  printPlan(SPENT_ROD_3);
  printPlan(NEW_ROD_4);
  printPlan(REACTOR_A);
  printPlan(REACTOR_B);
  printPlan(SPENT_ROD_4);
  printPlan(NEW_ROD_1);
  printPlan(REACTOR_B);
}

void printPlan(enum NavigatorPosition newPosition) {
  char commandBuffer[10];
  int length = navigator.buildPlan(newPosition, commandBuffer);
  Serial.println(length);
  for (int i = 0; i < length; i++) {
    Serial.print((int)commandBuffer[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
//  motion.armUp();
//  while (!motion.isDone()) motion.update();
//  motion.intakeIn();
//  while (!motion.isDone()) motion.update();
//  motion.armDown();
//  while (!motion.isDone()) motion.update();
//  motion.intakeOut();
//  while (!motion.isDone()) motion.update();
  
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

