#include "Arduino.h"
#include "Bluetoothh.h"
#include "Robott.h"
#include "LineSensor.h"
#include "Motor.h"

#define ENCODER_PIN1
#define ENCODER_PIN2

const char linesensor_pin = {};
const int ourteam = 12;
bool stopMovement;


Robott::Robott():
	mt(ENCODER_PIN1,ENCODER_PIN2),lsr(linesensor_pin),blueth(ourteam)
{


}



void mainloop(){
	switch(state){
		case 1:
			navigate_to_new_rod(); //state1
			break;
		case 2:
			navigate_to_spent_rod(); //state2
			break;
		case 3:
			navigate_to_empty_space(); //state3
			break;
		case 4:	
			get_rod(); //state4
			break;
		case 5:
			store_rod(); //state5
			break;
		default:
			halt();
	}

}

