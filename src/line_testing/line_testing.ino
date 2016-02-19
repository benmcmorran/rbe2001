#include "LineSensor.h"
#include <Servo.h>
#include <PID_v1.h>
#include <TimerOne.h>

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
PID myPID(&Input, &Output, &Setpoint, 30, 0, 1, DIRECT);

void setup() {
  Serial.begin(9600);
  left.attach(10);
  right.attach(11);
  pinMode(LED_PIN, OUTPUT);

  Input = 0;
  Setpoint = 0;
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-255, 255); 
}

int loopCount = 0;
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

  /*if (intersectionCount == 4) {
    leftSpeed = 90;
    rightSpeed = 90;
  } else*/ if (sensor.maxReading() > 200 && !isInIntersection) {
    Input = sensor.averageLinePosition();
    myPID.Compute();
    leftSpeed = 85 + Output;
    rightSpeed = 85 - Output;
  }

  safeMotorDrive(&left, leftSpeed);
  safeMotorDrive(&right, rightSpeed);

  loopCount++;
  if (loopCount == 10000) {
    Serial.println(millis());
  }
}

void safeMotorDrive(Servo *motor, int value) {
  motor->write(max(0, min(180, value)));
}

