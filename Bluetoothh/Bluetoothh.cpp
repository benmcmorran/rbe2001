#include "Bluetoothh.h"


Bluetoothh::Bluetoothh(int team) : pcol(byte(team)),stopMoving(false)
{
	Serial.begin(115200); // configure the console comm port to 115200 baud
  Serial1.begin(115200);  // the slave bluetooth module is also configured to 115200 baud
  bool unpacked[8] = {false,false,false,false,false,false,false,false};
	byte message[2]={0x00,0x00};
}

void Bluetoothh::sendHB(){
	byte data[3];
	byte pakage[10];
	int size;

	pcol.setDst(0x00);		
	size = pcol.createPkt(0x07, data, pakage);
    btmaster.sendPkt(pakage, size);   
}

void Bluetoothh::sendLowAlert(){
	byte data[3];
	byte pakage[10];
	int size;

	data[0] = 0x2C; 
	pcol.setDst(0x00); 
	size = pcol.createPkt(0x03, data, pakage);
    btmaster.sendPkt(pakage, size); 
}

void Bluetoothh::sendHighAlert(){
	byte data[3];
	byte pakage[10];
	int size;

	data[0] = 0xFF;
	pcol.setDst(0x00);		
	size = pcol.createPkt(0x03, data, pakage);
    btmaster.sendPkt(pakage, size); 
}

void Bluetoothh::checkstatus(){
	byte data[3];
	byte pakage[10];
	byte type;
	int size;

if(btmaster.readPacket(pakage)){
	if (pcol.getData(pakage, data, type)){
		if(pakage[4]== 0x00||pakage[4]==((byte)team)){
			switch(type){
				case 0x01: //storage availability
					message[0] = data[0];
					break;
				case 0X02: //supply
					message[1] = data[0];
					break;
				case 0x04: //stop movement
					stopMoving = true;
					break;
				case 0x05: //resume movement
					stopMoving = false;
					break;
				default:
					break;
			}
		}
	}	
}
	unpack();
}

void Bluetoothh::unpack(){		
		unpacked[0] = (bool) (message[0] & (byte) 0x01);
		unpacked[1] = (bool) (message[0] & (byte) 0x02) >>1;
		unpacked[2] = (bool) (message[0] & (byte) 0x04) >>2;
		unpacked[3] = (bool) (message[0] & (byte) 0x08) >>3;
		unpacked[4] = (bool) (message[1] & 0x01);
		unpacked[5] = (bool) (message[1] & 0x02) >>1;
		unpacked[6] = (bool) (message[1] & 0x04) >>2;
		unpacked[7] = (bool) (message[1] & 0x08) >>3;
}


//basically useless fuction, for debug purpose(maybe??)
void Bluetoothh::sentRobotStatus(int move, int grip, int operation){
	byte data[3];
	byte pakage[10];
	int size;

	switch(move){
		case 1:
			data[0] = 0x01; //stopped
			break;
		case 2:
			data[0] = 0x02; //moving(teleop)
			break;
		case 3:
			data[0] = 0x03; //moving(autonomous)
			break;		
		default:
			data[0] = 0x00;	
	}
	
	switch(grip){
		case 1:
			data[1] = 0x01; //No rod
			break;
		case 2:
			data[1] = 0x02; //Have rod
			break;		
		default:
			data[1] = 0x00;	
	}
	
	switch(operation){
		case 1:
			data[2] = 0x01; //Grip attempt in progress
			break;
		case 2:
			data[2] = 0x02; //Grip release in progress
			break;		
		case 3:
			data[2] = 0x03; //Driving to reactor rod
			break;		
		case 4:
			data[2] = 0x04; //Driving to storage area
			break;		
		case 5:
			data[2] = 0x05; //Driving to supply area
			break;		
		case 6:
			data[2] = 0x06; //No operation in progress
			break;		
		default:
			data[2] = 0x00;	
	}

	pcol.setDst(0x00);		
	size = pcol.createPkt(0x06, data, pakage);
    btmaster.sendPkt(pakage, size); 
}