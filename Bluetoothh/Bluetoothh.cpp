#include "Bluetoothh.h"
#include <BluetoothMaster.h>
#include <ReactorProtocol.h>

Bluetoothh::Bluetoothh() : pcol(byte(MY_TEAM)) //<--!!!the testboard id number
{
	// configure the console comm port to 115200 baud
  	Serial.begin(115200);
  	// the slave bluetooth module is also configured to 115200 baud
  	Serial1.begin(115200);
}

void Bluetoothh::sentHB(){
	byte data[3];
	byte pakage[10];
	int size;

	pcol.setDst(0x00);		
	size = pcol.createPkt(0x03, data, pakage);
    btmaster.sendPkt(pakage, size);   
}


void Bluetoothh::sentLowAlert(){
	byte data[3];
	byte pakage[10];
	int size;

	data[0] = 0x2C;		
	pcol.setDst(0x00);
	size = pcol.createPkt(0x03, data, pakage);
    btmaster.sendPkt(pakage, size); 

}


void Bluetoothh::sentHighAlert(){
	byte data[3];
	byte pakage[10];
	int size;

	data[0] = 0xFF;
	pcol.setDst(0x00);		
	size = pcol.createPkt(0x03, data, pakage);
    btmaster.sendPkt(pakage, size); 
}


char* Bluetoothh::checkstatue(){
	byte data[3];
	byte pakage[10];
	byte type;
	byte decode_message[]={0x00,0x00,0x00,0x00};
	int size;

	if(btmaster.readPacket(pakage)){
		if (pcol.getData(pakage, data, type)){
			if(pakage[4]== 0x00||pakage[4]==(byte)MY_TEAM){
				switch(type){
					case 0x01: //storage availability
						decode_message[0] = data[0];
						break;
					case 0X02: //supply
						decode_message[1] = data[0];
						break;
					case 0x04: //stop movement
						decode_message[2] = 0x01;
						break;
					case 0x05: //resume movement
						decode_message[3] = 0x01;
						break;
					default:
						break;
				}
			}
		}	
	}

	unpack(decode_message[]);
	return unpacked;
}

void Bluetoothh::unpack(byte decode_message[]){		
		unpacked[0] = (char) decode_message[0] & 0x01;
		unpacked[1] = (char) decode_message[0] & 0x02 >>1;
		unpacked[2] = (char) decode_message[0] & 0x04 >>2;
		unpacked[3] = (char) decode_message[0] & 0x08 >>3;
		unpacked[4] = (char) decode_message[1] & 0x01;
		unpacked[5] = (char) decode_message[1] & 0x02 >>1;
		unpacked[6] = (char) decode_message[1] & 0x04 >>2;
		unpacked[7] = (char) decode_message[1] & 0x08 >>3;
		unpacked[8] = (char) decode_message[2];
		unpacked[9] = (char) decode_message[3];
}



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