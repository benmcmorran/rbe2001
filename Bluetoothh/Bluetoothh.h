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
		void checkstatus();
		void sentRobotStatus(int move,int grip,int operation);
		
		//field
		int unpacked[9];
		byte decode_message[3];

	private:
		ReactorProtocol pcol; 
		BluetoothMaster btmaster;
		//store unpacked message
};

#endif