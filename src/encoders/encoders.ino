#include "Motor.h"

#include <Encoder.h>
#include <Servo.h>
#include <PID_v1.h>
#include <TimerOne.h>

Motor left(19, 20);
Motor right(18, 3);

long lastPrint;

void setup() {
  Motor::initialize();
  left.attach(10);
  right.attach(11);
  left.setSpeed(.5);
  right.setSpeed(.5);
  Serial.begin(9600);
  lastPrint = millis();
}

void loop() {
  left.update();
  right.update();

  long currentTime = millis();
  if (currentTime - lastPrint >= 1000) {    
    Serial.print("Left:\t");
    Serial.print(left.input);
    Serial.print("\t");
    Serial.print(left.output);
    Serial.print("\t");
    Serial.print(left.setpoint);
    Serial.println();
    lastPrint = currentTime;
  }
}

