#ifndef ROBOTT_H
#define ROBOTT_H

#include "Bluetoothh.h"
#include "Robott.h"
#include "LineSensor.h"
#include "Motor.h"
#include "Navigator.h"
#include <SimpleTimer.h>

enum RobotState{
	INITIAL,
	STOP,
	GO_SPENT_FIELD,
	GO_NEW_FIELD,
}rstate;

enum MotionState{
	STORE_ROD,
	GET_ROD,
}mstate;

class Robott{
	public:
		Robott(int ourteam);
		void main();

	private:
		Bluetoothh blue;
		Navigator navi;
		SimpleTimer sptimer1;
		bool isHB;
		bool roundNotDone;
		int round_counter;
		enum RobotState rstate;
		enum RobotState lrstate;
		//need a roller obj (maybe?)
};

#endif