#include "LineSensor.h"
#include <Servo.h>
#include <PID_v1.h>

#define LED_PIN 22

const int lineSensorPins[] = { 5, 6, 7, 1, 0, 2, 3, 4 };
LineSensor sensor(lineSensorPins);

const int positiveIntersectionThreshold = 300;
const int negativeIntsersectionThreshold = 200;
bool isInIntersection = false;
int intersectionCount = 0;

Servo left, right;
int leftSpeed, rightSpeed;

double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 20, 0, 0, DIRECT);

void setup() {
  Serial.begin(9600);
  left.attach(11);
  right.attach(10);
  pinMode(LED_PIN, OUTPUT);

  Input = 0;
  Setpoint = 0;
  myPID.SetMode(AUTOMATIC);
}

void loop() {
  digitalWrite(LED_PIN, isInIntersection);
  
  sensor.update();
  int averageReading = sensor.averageReading();
  if (isInIntersection && averageReading < negativeIntsersectionThreshold) {
    isInIntersection = false;
  } else if (!isInIntersection && averageReading > positiveIntersectionThreshold) {
    isInIntersection = true;
    intersectionCount++;
  }

  if (intersectionCount == 5) {
    leftSpeed = 90;
    rightSpeed = 90;
  } else if (sensor.maxReading() > 200) {
    float linePos = sensor.averageLinePosition();
    leftSpeed = 110 - (10 * linePos);
    rightSpeed = 110 + (10 * linePos);
  }
  /*else {
    Input = sensor.averageLinePosition();
    myPID.Compute();
    leftSpeed = 100 + Output;
    rightSpeed = 100 - Output;
  }*/

  left.write(leftSpeed);
  right.write(rightSpeed);
}
