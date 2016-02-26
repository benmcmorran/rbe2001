#include "Arduino.h"

Robott::Robott(int ourteam):
	blueth(ourteam),
	isHB(false),
	roundNotDone(true),
	round_counter(2),
	rstate(INITIAL),
	lrstate(INITIAL),
{
	sptimer1.setInterval(1500, sendHB_ISR);
}

Robott::main(){
	while(round_counter>0)
	{
		while(roundNotDone)
		{	
			switch(rstate)
			{

				case RAISE_ARM:

					break;
				case INITIAL:
					while(!blue.stopMoving){
						gotoReactor();
					}
					lrstate = INITIAL;
					break;
				case STOP:
					while(blue.stopMoving){
						blue.checkstatus();
					}
					rstate = lrstate;
					break;
			}
		}
		round_counter--;
	}
}

Robott::gotoReactor(){
	char command[10];
	int count;
	switch(round_counter){
		case 2:
			count = navi.buildPlan(REACTOR_A,command);
			break;
		case 1:
			count = navi.buildPlan(REACTOR_B,command);
			break;
		default:
			break;
	}	
	navi.executePlan(command,count);
	Navi.update();
	blue.checkstatus();
	if(isHB) blue.sendHB();
}


void sendHB_ISR(){
	isHB = true;
}