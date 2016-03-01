#include "Bluetoothh.h"
#include <BluetoothMaster.h>
#include <ReactorProtocol.h>
#include <Arduino.h>
#include <TimerOne.h>
#include <SimpleTimer.h>

SimpleTimer sp1;
Bluetoothh blue(0x0C);
bool isHB;
void sendHB_ISR(){
  isHB = true;
}

void setup() {
  pinMode(14,INPUT_PULLUP);
  pinMode(15,INPUT_PULLUP);
  
  Timer1.initialize(2000000);
  Timer1.attachInterrupt(sendHB_ISR);
  Serial.begin(115200); // configure the console comm port to 115200 baud
  Serial3.begin(115200);  
  isHB = false;
}


void loop() {
 if(isHB){
  blue.sendHB();
  isHB = false;
 }
 blue.checkstatus();
// blue.sendHighAlert();
//// delay(500);
// printBlue();
// blue.sendRobotStatus(255,190,205);
// delay(500);

}



void printBlue(){
  int i;
  for(i=0;i<4;i++){
    Serial.print("Storage");
    Serial.print(i);
    if(blue.getUnpack(i)){
      Serial.println("  true");
    }
    else{
      Serial.println("  false");
    }
  }
  for(i=4;i<8;i++){
    Serial.print("Supply");
    Serial.print(i-3);
    if(blue.getUnpack(i)){
      Serial.println("  true");
    }
    else{
      Serial.println("  false");
    }
  }
  Serial.print("StopMoving   ");
  Serial.println(blue.stopMoving);
  Serial.print("Team   ");
  Serial.print(blue.getTeam());
  Serial.println(""); 
}

