#ifndef BLUETOOTHH_H
#define BLUETOOTHH_H

#include <BluetoothMaster.h>
#include <ReactorProtocol.h>
#include <Arduino.h>
#include <SimpleTimer.h>

class Bluetoothh
{
	public:
		Bluetoothh(int team);
		void sendHB();
		void sendLowAlert();
		void sendHighAlert();
		void unpack();
		void checkstatus();
		void sentRobotStatus(int move,int grip,int operation);
		
		//field
		bool unpacked[8];
		byte message[3];
		bool stopMoving;
		bool isHB;

	private:
		ReactorProtocol pcol; 
		BluetoothMaster btmaster;
		SimpleTimer sptimer1;
};

#endif