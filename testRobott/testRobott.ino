#include <TimerThree.h>
#include <TimerOne.h>
#include "Robott.h"
#include "tone.h"

Robott R(12);

void setup() {
  pinMode(14,INPUT_PULLUP); //pull up one of the bluetooth pins
  pinMode(15,INPUT_PULLUP); //pull up one of the bluetooth pins
  Serial.begin(115200);
  Serial3.begin(115200);
  Timer1.initialize(2000000); //let timer1 generate an interrupt every 2 seconds
  Timer1.attachInterrupt(sendHB_ISR); //attach timer1 interrupt to sendHB_ISR
  Timer3.initialize(1000000); //let timer3 generate an interrupt every second
  Timer3.attachInterrupt(sendAlert_ISR); //attach timer3 interrupt to sendAlert_ISR
  R.initialize();
  R.main();
}

void loop() {
  play_rtttl(song);
}

void sendHB_ISR(){
  R.setIsHB(true); //set the send hearbeat flag to true
}
void sendAlert_ISR(){
  digitalWrite(alertLED,LOW); //turn off the physical alert LED
  R.isAlert=true; //set the send hearbeat flag to true
}

