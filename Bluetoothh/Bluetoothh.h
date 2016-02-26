#ifndef BLUETOOTHH_H
#define BLUETOOTHH_H

#include <BluetoothMaster.h>
#include <ReactorProtocol.h>
#include <Arduino.h>

class Bluetoothh
{
	public:
		Bluetoothh(int team);
		void sendHB();
		void sendLowAlert();
		void sendHighAlert();
		void checkstatus();
		void sentRobotStatus(int move,int grip,int operation);
		
		//field
		bool unpacked[8];
		byte message[2];
		bool stopMoving;

	private:
		void unpack();
		ReactorProtocol pcol; 
		BluetoothMaster btmaster;
};

#endif