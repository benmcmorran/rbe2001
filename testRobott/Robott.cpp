#include <Arduino.h>
#include "Robott.h"

Robott::Robott(int ourteam):blue((byte)ourteam),navi(&motion)
{
}

/**Initializing our robot
**/
void Robott::initialize(){
	do{
		blue.checkstatus(); //try to connect to the bluetooth
		if(isHB){
			blue.sendHB();
			isHB = false;
		}
	}while(!blue.isConnected()); //if its connect, break the checking loop
	motion.initialize();	//attach all the pins 
  motion.armUp(false); wait(false); //raise the roller up
  motion.trackToBump(); wait(false); //locate reactor A and build a map
  pinMode(alertLED,OUTPUT); //register  the alert LED pin
}

/**The main funcion executed in our nuclear reactor challenge
**/
void Robott::main(){
	while(round_counter>0) //there are 2 rounds
	{
		while(roundNotDone) //sendAlertflag indicating a round has been done
		{	
			switch(robot_state) //begin of the state machine
			{
				case GO_TO_REACTOR:
					gotoReactor(); //go to reactor A or B, depends on the round
					updateState(); //update the state
					break;
				case GO_SPENT_FIELD: 
				case GO_NEW_FIELD:
					gotoField(); //go to the spent field or new field, depends on the state
					updateState(); //update the state
					break;
				case STORE_NEW_ROD:
					storeNewRod(); //arm and roller motion: store the new rod in to the reactor
					updateState(); //update the state 
					break;
				case STORE_SPENT_ROD:
					storeSpentRod(); //arm and roller motion: store the spent rod in the spent field
					updateState(); //update the state
					break;
				case GET_NEW_ROD:
					getNewRod(); //arm and roller motion: get the new rod from the new field
					updateState(); //update the state
					break;
				case GET_SPENT_ROD:
					getSpentRod(); //arm and roller motion: get the spend rod from the reactor
					updateState(); //update the state
					break;
				default:
					blue.sendRobotStatus(171,171,171); //impossible default status, send status for debugging reason
					break;
			}
		}
		round_counter--; //if a round is done, decrement round counter
		roundNotDone=true; //and set clear the round having done sendAlertflag
	}
}

/**Updating the motion until the motion has been done
	@param sendAlertflag Flag inidicating if it's the motion that need to send alert
**/
void Robott::wait(bool sendAlertflag) {//if we need to send alert in a specific motion, pass a true flag
  while (!motion.isDone()) { 
    motion.update();	//update the motion if motion has not been done
    if(isHB){					//check if it's time to send heart beat
				blue.sendHB();
				isHB = false;
		}
		motion.update(); 
		if(sendAlertflag){//if the sending alert is needed
	    switch(robot_state){ //sending alert also depends on the current state
	    	case GET_SPENT_ROD: 
	    	case STORE_SPENT_ROD:
	    		sendAlert(0); //send low alert
	    		break;
	    	case STORE_NEW_ROD:
	    	case GET_NEW_ROD:
	    		sendAlert(1); //send high alert
	    		break;
				default:
					blue.sendRobotStatus(190,190,190); //impossible default status, send status for debugging reason
					break; 
	    }
	  }
  }
}


/**Going to the reactor
**/
void Robott::gotoReactor(){
		if(round_counter==2){ //if it is round 2, which is the first round
			count = navi.buildPlan(REACTOR_A,command);	//build the plan driving to the reactor A
		}else{								//if it is round 1, which is the second round
			count = navi.buildPlan(REACTOR_B,command);	//build the plan driving to the reactor A
		}
		navi.executePlan(command,count); //execute the plan it built
		while(!navi.isDone()){ //while the plan has not been exucuted sucessfully
			updateNavi(5); //update the drive line motion
			blue.checkstatus(); //check bluetooth and update bluetooth messages
			if(isHB){	//check if its time to send hearbeat message
				blue.sendHB(); //send hearbeat message
				isHB = false; //flip the flag
			}
			sendAlert(1); //send high alert message
			miniStopState(); //function that responds to the stop/resume moving message
		}
}

/**Driving the robot to spent/new field
**/
void Robott::gotoField(){
		for(int i=0;i<15;i++){
			blue.checkstatus(); //check bluetooth and update bluetooth messages
			if(isHB){	//check if its time to send hearbeat message
				blue.sendHB(); //send hearbeat message
				isHB = false; //flip the flag
			}
		}
		enum NavigatorPosition dest = priorDestination();
		count = navi.buildPlan(dest,command);
		navi.executePlan(command,count);
		while(!navi.isDone()){
			updateNavi(15);	//update the drive line motion
			blue.checkstatus(); //check bluetooth and update bluetooth messages
			if(isHB){	//check if its time to send hearbeat message
				blue.sendHB(); //send hearbeat message
				isHB = false; //flip the flag
			}
			if(robot_state==GO_SPENT_FIELD){
				sendAlert(0); //send low alert message
			}
			miniStopState(); //function that responds to the stop/resume moving message
		}
}

/**Calculating the prior position that a robot should go when it's driving to the next field
	@return The position index in NavigatorPosition index form
**/
enum NavigatorPosition Robott::priorDestination(){
	int i;
	if(round_counter==2){ //go to REACTOR_A round
			if(robot_state==GO_SPENT_FIELD){ //if the robot is going to the spent field
				for(i=3;i>=0;i--){ //loop through storage place from 3 to 0 
					if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i); //check if there is an empty space
				}																															 //if it is, return the place index in NavigatorPosition index form
			}
			else{		//if the robot is going to a new field
				if(checkOtherSide()) return (enum NavigatorPosition)(11-navi.getIndex()); //check if the opposite side has a rod first
				else{	//if the opposite site does not have a rod
					for(i=4;i<=7;i++){ //loop through the new field message from 4 to 7 to see if there is a rod
						if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i); //if there is a rod, return the rod position in NavigatorPosition index form
					}
				}
			}
	}
	else{	//go to REACTOR_B round
			if(robot_state==GO_SPENT_FIELD){
				for(i=0;i<=3;i++){	//loop through storage place from 0 to 3
					if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i);	//check if there is an empty space
				}																																//if it is, return the place index in NavigatorPosition index form
			}
			else{//state = GO_NEW_ROD
				if(checkOtherSide()) return (enum NavigatorPosition)(11-navi.getIndex()); //check if the opposite side has a rod first
				else{
					for(i=7;i>=4;i--){ //loop through the new field message from 7 to 4 to see if there is a rod
						if(blue.getUnpack(i)) return (enum NavigatorPosition) (2+i); //if there is a rod, return the rod position in NavigatorPosition index form
					}
				}
			}
	}	
}

/**Check if the opposite side of field has a rod
	@param Boolean indicating if the opposite side is available
**/
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

/**Check if receive a stop/resume message to our robot and respond to it
**/
void Robott::miniStopState(){
	if(blue.stopMoving){ //if receive a stop message
		motion.stop(); //robot stop
	}
	else{ //if receive a resume message
		motion.resume(); //robot resume moving
	}
}

/**Store a new rod into the reactor
**/
void Robott::storeNewRod(){
  motion.armDown(); wait(true); //roll the arm own while sending a high alert message
  motion.intakeOut(); wait(true); //store the rod while sending a high alert message
  motion.armUp(true); wait(false); //raise the arm up
}

/**Store a spent rod into the storage place
**/
void Robott::storeSpentRod(){
	motion.intakeOut(); wait(true); //store the rod while sending a low alert message
}

/**Get a new rod from the new field
**/
void Robott::getNewRod(){
	motion.intakeIn(); wait(true); //take out a new rod from the new field while sending a high alert message
}

/**Get a spent rod fromt the reactor
**/
void Robott::getSpentRod(){
	motion.armDown(); wait(false); //roll the arm down
  motion.intakeIn(); wait(true); //take out a spent rod from the reactor while sending a low alert message
  motion.armUp(false); wait(true); //raise the arm up while sending a low alert message
}

/**Set the heart beat flag
	@param outsideHB Flag indicating if it's the time to send heartbeat message
**/
void Robott::setIsHB(bool outsideHB){
	isHB = outsideHB;
}

/**Update the state after finish one state
**/
void Robott::updateState(){
	stateIndex++; //increment state index
	if(stateIndex>=8){ //if the all of the states have been done
		stateIndex = 0; //reset the state to the first one
		roundNotDone = false; //and start a new round
	}
	robot_state = statelist[stateIndex]; //if all of the states haven't been done, move to next state
}

/**Send alert messages
	@param level 0 indicating a low level alert message, 1 indicating a high level alert message
**/
void Robott::sendAlert(int level){
	if(level==0){ //if the robot should send a low level alert
		if(isAlert){ //if it's time to send alert
			blue.sendLowAlert(); //send a low level alert message
    	isAlert = false; //flip the alert flag down
    	digitalWrite(alertLED,HIGH);//turn on the physical alert LED
		}
	}
	else{ //if the robot should send a high level alert
		if(isAlert && stateIndex!=0){
			blue.sendHighAlert(); //send a high level alert message
			digitalWrite(alertLED,HIGH); //turn on the physical alert LED
    	isAlert = false; //flip the alert flag down
		}
	}
}
