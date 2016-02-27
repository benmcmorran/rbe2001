#include "Arduino.h"

void sendHB_ISR(){
	isHB = true;
}

Robott::Robott(int ourteam):
	blue(ourteam),
	isHB(false),
	roundNotDone(true),
	round_counter(2),
	stateIndex(0),
	robot_state(GO_TO_REACTOR),
{
	sptimer1.setInterval(1500, sendHB_ISR);
	statelist = {GO_TO_REACTOR,GET_SPENT_ROD,GO_SPENT_FIELD,
		STORE_SPENT_ROD,GO_NEW_FIELD,GET_NEW_ROD,GO_TO_REACTOR,STORE_NEW_ROD};
}

void Robott::main(){
	while(round_counter>0)
	{
		while(roundNotDone)
		{	
			switch(robot_state)
			{
				case GO_TO_REACTOR:
					while(stateNotDone){
						gotoReactor();
						conditional_miniStopState();
					}
					stateIndex++;
					robot_state = statelist[(stateIndex>=8)?0:stateIndex];
					break;

				case GO_SPENT_FIELD: case GO_NEW_FIELD:
					enum NavigatorPosition dest = priorDestination();
					while(stateNotDone){
						gotoField(dest);
						conditional_miniStopState();
					}
					stateIndex++;
					robot_state = statelist[(stateIndex>=8)?0:stateIndex];
					break;

				case STORE_NEW_ROD:


					break;

				case STORE_SPENT_ROD:
					while(stateNotDone){

					}
					stateIndex++;
					robot_state = statelist[(stateIndex>=8)?0:stateIndex];
					break;

				case GET_NEW_ROD:
					while(stateNotDone){


					}
					robot_state = statelist[++stateIndex];
					break;

				case GET_SPENT_ROD:

					break;

				default:
					blue.sendRobotStatus(171,171,171);
					break;
			}
		}
		round_counter--;
	}
}

void Robott::gotoReactor(){
		char command[10];
		int count=0;
		switch(round_counter){
			case 2:
				count = navi.buildPlan(NavigatorPosition.REACTOR_A,command);
				break;
			case 1:
				count = navi.buildPlan(NavigatorPosition.REACTOR_B,command);
				break;
			default:
				blue.sendRobotStatus(255,255,255);
				break;
		}	
		navi.executePlan(command,count);
		updateNavi(5);
		blue.checkstatus();
		if(isHB){
			blue.sendHB();
			isHB = false;
		}
}


void Robott::gotoField(enum NavigatorPosition dest){
		char command[10];
		int count=0;
		count = navi.buildPlan(dest,command);
		navi.executePlan(command,count);
		updateNavi(5);		blue.checkstatus();
		if(isHB){
			blue.sendHB();
			isHB = false;
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
		Navi.update();
	}
}

void Robott::conditional_miniStopState(){
	while(blue.stopMoving){
		blue.checkstatus();
		if(isHB){
			blue.sendHB();
			isHB = false;
		}
	}
}
