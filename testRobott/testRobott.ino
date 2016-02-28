#include <TimerOne.h>
#include <SimpleTimer.h>
#include "Robott.h"

Robott R(12);
SimpleTimer sp;
void setup() {
  pinMode(14,INPUT_PULLUP);
  pinMode(15,INPUT_PULLUP);
  Serial.begin(115200);
  Serial3.begin(115200);
  Timer1.initialize(2000000);
  Timer1.attachInterrupt(sendHB_ISR);
  sp.setInterval(1000,sendAlert_ISR);
  R.initialize();
}

void loop() {
  R.main();
}

void sendHB_ISR(){
  R.setIsHB(true);
}
void sendAlert_ISR(){
  R.isAlert=true;
}

