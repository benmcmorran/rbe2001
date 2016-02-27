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
		void sendRobotStatus(int move,int grip,int operation);
		bool getUnpack(int bluetoothIndex);

		//field
		bool stopMoving;

	private:
		void unpack();
		byte message[2];
		bool unpacked[8];

		ReactorProtocol pcol; 
		BluetoothMaster btmaster;
};

#endif