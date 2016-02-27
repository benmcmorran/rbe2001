#ifndef ROBOTT_H
#define ROBOTT_H

#include "Bluetoothh.h"
#include "Navigator.h"
#include <SimpleTimer.h>

enum RobotState{
	GO_TO_REACTOR,
	GET_SPENT_ROD,
	GO_SPENT_FIELD,
	STORE_SPENT_ROD,
	GO_NEW_FIELD,
	GET_NEW_ROD,
	STORE_NEW_ROD,
};


class Robott{
	public:
		Robott(int ourteam);
		void main();

	private:
		enum NavigatorPosition priorDestination();
		bool checkOtherSide();
		void updateNavi(int n);
		void conditional_miniStopState();
		void gotoField(enum NavigatorPosition dest);
		void gotoReactor();


		
		Bluetoothh blue;
		Navigator navi;
		SimpleTimer sptimer1;

		bool isHB;
		bool roundNotDone;
		int round_counter;
		enum RobotState robot_state;
		enum RobotState statelist[8];
		int stateIndex;
};

#endif