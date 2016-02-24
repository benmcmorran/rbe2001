#include "LineSensor.h"
#include <Encoder.h>
#include <Servo.h>
#include <PID_v1.h>
#include <TimerOne.h>

#define LED_PIN 22
#define FRONT_BUMPER_PIN 25

const int lineSensorPins[] = { 5, 6, 7, 1, 0, 2, 3, 4 };
LineSensor sensor(lineSensorPins);

const int positiveIntersectionThreshold = 300;
const int negativeIntsersectionThreshold = 200;
bool isInIntersection = false;
int intersectionCount = 0;

Servo left, right;
int leftSpeed, rightSpeed;

double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 20, 0, 3, DIRECT);

Encoder leftE(19, 20);
Encoder rightE(18, 3);

bool isTurning = false;

void setup() {
  Serial.begin(9600);
  left.attach(10);
  right.attach(11);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FRONT_BUMPER_PIN, INPUT_PULLUP);

  Input = 0;
  Setpoint = 0;
  myPID.SetOutputLimits(-255, 255);
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
    leftE.write(0);
    rightE.write(0);
    isTurning = true;
  }

  if (digitalRead(FRONT_BUMPER_PIN) == LOW) {
    leftSpeed = 90;
    rightSpeed = 90;
  } else if (isTurning) {
    myPID.SetMode(MANUAL);
    leftSpeed = leftE.read() > 320 ? 90 : 75;
    rightSpeed = rightE.read() < -150 ? 90 : 105;
    isTurning = !(leftE.read() > 320 && rightE.read() < -150);
  } else if (sensor.isLineDetected()/*sensor.maxReading() > 200 && !isInIntersection && millis() - intersectionExitTime > 500*/) {
    Input = sensor.averageLinePosition();
    myPID.SetMode(AUTOMATIC);
    myPID.Compute();
    leftSpeed = 80 + Output;
    rightSpeed = 80 - Output;
  } else {
    leftSpeed = 80;
    rightSpeed = 80;
    myPID.SetMode(MANUAL);
  }

  safeMotorDrive(&left, leftSpeed);
  safeMotorDrive(&right, rightSpeed);
}

void safeMotorDrive(Servo *motor, int value) {
  motor->write(max(0, min(180, value)));
}

