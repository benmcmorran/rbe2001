#ifndef BLUETOOTHH_H
#define BLUETOOTHH_H

#include <BluetoothMaster.h>
#include <ReactorProtocol.h>
#include <Arduino.h>

class Bluetoothh
{
	public:
		Bluetoothh(byte teamn);
		void sendHB();
		void sendLowAlert();
		void sendHighAlert();
		void checkstatus();
		void sendRobotStatus(int move,int grip,int operation);
		bool getUnpack(int bluetoothIndex);

		//field
		bool stopMoving=false;

	private:
		void unpack();
		byte message[2]={0x00,0x00};
		bool unpacked[8]={true,false,false,false,true,true,false,false};
    byte team;
		ReactorProtocol pcol; 
		BluetoothMaster btmaster;
};

#endif
