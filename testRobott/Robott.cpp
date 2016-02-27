#include "Arduino.h"
#include "Robott.h"

Robott::Robott(int ourteam):blue((byte)ourteam),navi(&motion)
{
}

void Robott::initialize(){
	motion.initialize();
  motion.armUp(); wait();
  motion.trackToBump(); wait();
  motion.armDown(); wait();
  motion.intakeIn(); wait();
  motion.armUp(); wait();
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
					stateIndex++;
					if(stateIndex>=8){
						stateIndex = 0;
						roundNotDone = false;
					}
					robot_state = statelist[stateIndex];
					break;
				case GO_SPENT_FIELD: case GO_NEW_FIELD:
					gotoField();
					stateIndex++;
					if(stateIndex>=8){
						stateIndex = 0;
						roundNotDone = false;
					}
					robot_state = statelist[stateIndex];
					break;
				case STORE_NEW_ROD:
					storeNewRod();
					stateIndex++;
					if(stateIndex>=8){
						stateIndex = 0;
						roundNotDone = false;
					}
					break;
				case STORE_SPENT_ROD:
					storeSpentRod();
					stateIndex++;
					if(stateIndex>=8){
						stateIndex = 0;
						roundNotDone = false;
					}
					robot_state = statelist[stateIndex];
					break;
				case GET_NEW_ROD:
					getNewRod();
					stateIndex++;
					if(stateIndex>=8){
						stateIndex = 0;
						roundNotDone = false;
					}
					robot_state = statelist[stateIndex];
					break;
				case GET_SPENT_ROD:
          Serial.println("IN GET_SPENT_ROD STATE");
					getSpentRod();
					stateIndex++;
					if(stateIndex>=8){
						stateIndex = 0;
						roundNotDone = false;
					}
					robot_state = statelist[stateIndex];
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

void Robott::storeNewRod(){
  motion.armDown(); wait();
  motion.intakeOut(); wait();
  motion.armUp(); wait();
}

void Robott::storeSpentRod(){
	motion.intakeOut(); wait();
}

void Robott::getNewRod(){
	motion.intakeIn(); wait();
}

void Robott::getSpentRod(){
	motion.armDown(); wait();
  motion.intakeIn(); wait();
  motion.armUp(); wait();
}

void Robott::wait() {
  while (!motion.isDone()) {
    motion.update();
    if(isHB){
				blue.sendHB();
				isHB = false;
		}
		motion.update();
    switch(robot_state){
    	case GO_SPENT_FIELD: case STORE_SPENT_ROD: case GET_SPENT_ROD:
    		blue.sendLowAlert();
    		break;
    	case GO_TO_REACTOR: case GET_NEW_ROD: case STORE_NEW_ROD:
    		if(stateIndex!=0)
    			blue.sendHighAlert();
    		break;
			default:
				blue.sendRobotStatus(190,190,190);
				break; 
    }
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
			Serial.println("I am in the gotoReactor LOOP"); ////////////////////////////////////////////////////////////
			blue.checkstatus();
			if(isHB){
				blue.sendHB();
				isHB = false;
			}
			conditional_miniStopState();		
		}
}


void Robott::gotoField(){
		enum NavigatorPosition dest = priorDestination();
		Serial.println("The prior Dest is");
		Serial.print(dest); ////////////////////////////////////////////////////////////////////
		count = navi.buildPlan(dest,command);
		navi.executePlan(command,count);
		while(!navi.isDone()){
			Serial.println("I am in the gotoField LOOP"); ////////////////////////////////////////////////////////////
			updateNavi(15);	
			blue.checkstatus();
			if(isHB){
				blue.sendHB();
				isHB = false;
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

void Robott::setIsHB(bool outsideHB){
	isHB = outsideHB;
}

bool Robott::getHB(){
	return isHB;
}
