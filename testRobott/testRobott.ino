#include <TimerThree.h>
#include <TimerOne.h>
//#include <SimpleTimer.h>
#include "Robott.h"
#include "tone.h"

Robott R(12);

void setup() {
  pinMode(14,INPUT_PULLUP);
  pinMode(15,INPUT_PULLUP);
  Serial.begin(115200);
  Serial3.begin(115200);
  Timer1.initialize(2000000);
  Timer1.attachInterrupt(sendHB_ISR);
  Timer3.initialize(1000000);
  Timer3.attachInterrupt(sendAlert_ISR);
  R.initialize();
  R.main();
}

void loop() {
  play_rtttl(song);
}

void sendHB_ISR(){
  R.setIsHB(true);
}
void sendAlert_ISR(){
  digitalWrite(22,LOW);
  R.isAlert=true;
}

