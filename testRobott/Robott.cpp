#include <Arduino.h>
#include "Robott.h"

Robott::Robott(int ourteam):blue((byte)ourteam),navi(&motion)
{
}

void Robott::initialize(){
	do{
		blue.checkstatus();
		if(isHB){
			blue.sendHB();
			isHB = false;
		}
	}while(!blue.isConnected());
	motion.initialize();
  motion.armUp(); initialWait();
  motion.trackToBump(); initialWait();
  pinMode(22,OUTPUT);
}

void Robott::main(){
	while(round_counter>0)
	{
		while(roundNotDone)
		{	
			switch(robot_state)
			{
				case GO_TO_REACTOR:
					gotoReactor();
					updateState();
					break;
				case GO_SPENT_FIELD: 
				case GO_NEW_FIELD:
					gotoField();
					updateState();
					break;
				case STORE_NEW_ROD:
					storeNewRod();
					updateState();
					break;
				case STORE_SPENT_ROD:
					storeSpentRod();
					updateState();
					break;
				case GET_NEW_ROD:
					getNewRod();
					updateState();
					break;
				case GET_SPENT_ROD:
          Serial.println("IN GET_SPENT_ROD STATE");
					getSpentRod();
					updateState();
					break;
				default:
					blue.sendRobotStatus(171,171,171);
					break;
			}
		}
		round_counter--;
		roundNotDone=true;
	}
}

void Robott::wait(bool flag) {
  while (!motion.isDone()) {
    motion.update();
    if(isHB){
				blue.sendHB();
				isHB = false;
		}
		motion.update();
		if(flag){
	    switch(robot_state){
	    	case GET_SPENT_ROD: 
	    	case STORE_SPENT_ROD:
	    		sendAlert(0);
	    		break;
	    	case STORE_NEW_ROD:
	    	case GET_NEW_ROD:
	    		sendAlert(1);
	    		break;
				default:
					blue.sendRobotStatus(190,190,190);
					break; 
	    }
	  }
  }
}

void Robott::initialWait(){
	while (!motion.isDone()) {
    motion.update();
  }
}

void Robott::gotoReactor(){
		if(round_counter==2){		
			count = navi.buildPlan(REACTOR_A,command);		
		}else{
			count = navi.buildPlan(REACTOR_B,command);
		}
		navi.executePlan(command,count);
		while(!navi.isDone()){
			updateNavi(5);
			blue.checkstatus();
			if(isHB){
				blue.sendHB();
				isHB = false;
			}
			sendAlert(1);
			conditional_miniStopState();		
		}
}


void Robott::gotoField(){
		for(int i=0;i<15;i++){
			blue.checkstatus();
			if(isHB){
				blue.sendHB();
				isHB = false;
			}
		}
		enum NavigatorPosition dest = priorDestination();
		count = navi.buildPlan(dest,command);
		navi.executePlan(command,count);
		while(!navi.isDone()){
			updateNavi(15);	
			blue.checkstatus();
			if(isHB){
				blue.sendHB();
				isHB = false;
			}
			if(robot_state==GO_SPENT_FIELD){
				sendAlert(0);
			}
			conditional_miniStopState();
		}
}

//only use in GO_SPENT_FIELD and GO_NEW_FIELD
enum NavigatorPosition Robott::priorDestination(){
	int i;
	if(round_counter==2){ //go to REACTOR_A round
			if(robot_state==GO_SPENT_FIELD){
				for(i=3;i>=0;i--){
					if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i);
				}
			}
			else{//state = GO_NEW_ROD
				if(checkOtherSide()) return (enum NavigatorPosition)(11-navi.getIndex());
				else{
					for(i=4;i<=7;i++){
						if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i);
					}
				}
			}
	}
	else{//go to REACTOR_B round
			if(robot_state==GO_SPENT_FIELD){
				for(i=0;i<=3;i++){
					if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i);
				}
			}
			else{//state = GO_NEW_ROD
				if(checkOtherSide()) return (enum NavigatorPosition)(11-navi.getIndex());
				else{
					for(i=7;i>=4;i--){
						if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i);
					}
				}
			}
	}	
}

bool Robott::checkOtherSide(){//only uses in GET_NEW_ROD
	int bluetoothIndex = (int) navi.getIndex()-2;
	int OppositeIndex = 7 - bluetoothIndex;
	return blue.getUnpack(OppositeIndex);
}

void Robott::updateNavi(int n){
	int i;
	for(i=0;i<n;i++){
		navi.update();
	}
}

void Robott::conditional_miniStopState(){
	while(blue.stopMoving){
		blue.checkstatus(); 
		Serial.println("I AM NOT STUCK IN THIS LOOP");
		if(isHB){
			blue.sendHB();
			setIsHB(false);
		}
	}
}


void Robott::storeNewRod(){
  motion.armDown(); wait(true);
  motion.intakeOut(); wait(true);
  motion.armUp(); wait(false);
}

void Robott::storeSpentRod(){
	motion.intakeOut(); wait(true);
}

void Robott::getNewRod(){
	motion.intakeIn(); wait(true);
}

void Robott::getSpentRod(){
	motion.armDown(); wait(false);
  motion.intakeIn(); wait(true);
  motion.armUp(); wait(true);
}


void Robott::setIsHB(bool outsideHB){
	isHB = outsideHB;
}

void Robott::updateState(){
	stateIndex++;
	if(stateIndex>=8){
		stateIndex = 0;
		roundNotDone = false;
	}
	robot_state = statelist[stateIndex];
}

void Robott::sendAlert(int level){
	if(level==0){
		if(isAlert){
			blue.sendLowAlert();
    	isAlert = false;
    	digitalWrite(22,HIGH);
		}
	}
	else{
		if(isAlert && stateIndex!=0){
			blue.sendHighAlert();
			digitalWrite(22,HIGH);
    	isAlert = false;
		}
	}
}
