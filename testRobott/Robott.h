#ifndef ROBOTT_H
#define ROBOTT_H

#include "Motion.h"
#include "Bluetoothh.h"
#include "Navigator.h"

enum RobotState{
	GO_TO_REACTOR=999,
	GET_SPENT_ROD=998,
	GO_SPENT_FIELD=997,
	STORE_SPENT_ROD=996,
	GO_NEW_FIELD=995,
	GET_NEW_ROD=994,
	STORE_NEW_ROD=993,
};


class Robott{
	public:
		Robott(int ourteam);
		void main();
    void setIsHB(bool isHB);
    void initialize();
    volatile bool isAlert = false;

	private:
		enum NavigatorPosition priorDestination();
		bool checkOtherSide();
		void updateNavi(int n);
		void conditional_miniStopState();
		void gotoField();
		void gotoReactor();
		void storeNewRod();
		void storeSpentRod();
		void getNewRod();
		void getSpentRod();
		void wait();
		void updateState();
		Bluetoothh blue;
		Navigator navi;
		Motion motion;

		bool roundNotDone=true;
		volatile bool isHB=false;
		int round_counter=2;
		int stateIndex=1;
    int count=0;
    char command[10];
		enum RobotState robot_state = GET_SPENT_ROD;
		enum RobotState statelist[8] = {GO_TO_REACTOR,GET_SPENT_ROD,GO_SPENT_FIELD,
  	STORE_SPENT_ROD,GO_NEW_FIELD,GET_NEW_ROD,GO_TO_REACTOR,STORE_NEW_ROD};;
		};

#endif
