#include <SimpleTimer.h>
#include "Robott.h"

Robott R(12);
SimpleTimer sptimer1;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  sptimer1.setInterval(1500, sendHB_ISR);
  R.initialize();
}
void loop() {
  R.main();
}
void sendHB_ISR(){
  R.setIsHB(true);
}
