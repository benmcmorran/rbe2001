#ifndef BLUETOOTHH_H
#define BLUETOOTHH_H

#define MY_TEAM 20

#include <BluetoothMaster.h>
#include <ReactorProtocol.h>

class Bluetoothh
{
	public:
		Bluetoothh();
		void sentHB();
		void sentLowAlert();
		void sentHighAlert();
		void unpack();
		void Bluetoothh::sentRobotStatus(int move,int grip,int operation);

	private:
		ReactorProtocol pcol; 
		BluetoothMaster btmaster;
		char unpacked[10]; 	//store unpacked message
};

#endif