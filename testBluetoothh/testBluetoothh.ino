#include "Bluetoothh.h"
#include <BluetoothMaster.h>
#include <ReactorProtocol.h>
#include <Arduino.h>

void setup() {
  Serial.begin(115200); // configure the console comm port to 115200 baud
  Serial1.begin(115200);  
}
Bluetoothh blue(0x11);

void loop() {
 for(int i=0;i<30;i++){
 blue.checkstatus();
 }
 delay(500);
 blue.sendHB();
 delay(100);
 blue.sendLowAlert();
 delay(500);
 blue.sendHighAlert();
 delay(500);
 printBlue();
 blue.sendRobotStatus(255,255,255);
}

void printBlue(){
  int i;
  for(i=0;i<4;i++){
    Serial.print("Storage");
    Serial.print(i);
    if(blue.getUnpack(i)){
      Serial.print("  true");
      Serial.println("");
    }
    else{
      Serial.print("  false");
      Serial.println("");
    }
  }
    for(i=4;i<8;i++){
    Serial.print("Supply");
    Serial.print(i-3);
    if(blue.getUnpack(i)){
      Serial.print("  true");
      Serial.println("");
    }
    else{
      Serial.print("  false");
      Serial.println("");
    }
  }
  Serial.println(""); 
}

