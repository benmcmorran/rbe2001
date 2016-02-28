#include "Bluetoothh.h"
#include <Arduino.h>

Bluetoothh::Bluetoothh(byte teamn):team(teamn),pcol(teamn)
{
}

void Bluetoothh::sendHB(){
	byte data[3];
	byte package[10];
	int size;

	pcol.setDst(0x00);		
	size = pcol.createPkt(0x07, data, package);
    btmaster.sendPkt(package, size);   
}

void Bluetoothh::sendLowAlert(){
	byte data[3];
	byte package[10];
	int size;

	data[0] = 0x2C; 
	pcol.setDst(0x00); 
	size = pcol.createPkt(0x03, data, package);
    btmaster.sendPkt(package, size); 
}

void Bluetoothh::sendHighAlert(){
	byte data[3];
	byte package[10];
	int size;

	data[0] = 0xFF;
	pcol.setDst(0x00);		
	size = pcol.createPkt(0x03, data, package);
    btmaster.sendPkt(package, size); 
}

void Bluetoothh::checkstatus(){
	byte data[3];
	byte package[10];
	byte type;
	int size;
	if(btmaster.readPacket(package)){
		if (pcol.getData(package, data, type)){
//    Serial.println("GetData");
			if(package[4]== 0x00||package[4]==team){
//       Serial.println("Received");
//        Serial.print("type    ");
//        Serial.println(type);
				switch(type){
					case 0x01: //storage availability
						message[0] = data[0];
						break;
					case 0X02: //supply
						message[1] = data[0];
						break;
					case 0x04: //stop movement
						stopMoving = true;
            Serial.println("Received stopmoving");
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
		unpacked[3] = (bool) !((message[0] & (byte) 0x01)); //isEmpty?
//    Serial.print(unpacked[3]);
		unpacked[2] = (bool) !((message[0] & (byte) 0x02) >>1);
//    Serial.print(unpacked[2]);
		unpacked[1] = (bool) !((message[0] & (byte) 0x04) >>2);
//    Serial.print(unpacked[1]);
		unpacked[0] = (bool) !((message[0] & (byte) 0x08) >>3);
//    Serial.print(unpacked[0]);
//    Serial.println("");
		unpacked[4] = (bool) (message[1] & 0x01);
//    Serial.print(unpacked[4]);
		unpacked[5] = (bool) ((message[1] & 0x02) >>1);
//    Serial.print(unpacked[5]);
		unpacked[6] = (bool) ((message[1] & 0x04) >>2);
//    Serial.print(unpacked[6]);
		unpacked[7] = (bool) ((message[1] & 0x08) >>3); //hasRod?
//    Serial.print(unpacked[7]);
//    Serial.println("");
//    Serial.println("Message[1]");
//    Serial.println(message[1]);
}

bool Bluetoothh::getUnpack(int bluetoothIndex){
	return unpacked[bluetoothIndex];
}

//basically useless fuction, for debug purpose(maybe??)
void Bluetoothh::sendRobotStatus(int move, int grip, int operation){
	byte data[3];
	byte package[10];
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
			data[0] = byte(move);	
	}
	
	switch(grip){
		case 1:
			data[1] = 0x01; //No rod
			break;
		case 2:
			data[1] = 0x02; //Have rod
			break;		
		default:
			data[1] = byte(grip);	
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
			data[2] = byte(operation);	
	}

	pcol.setDst(0x00);		
	size = pcol.createPkt(0x06, data, package);
    btmaster.sendPkt(package, size); 
}

byte Bluetoothh::getTeam(){
  return team;
}

bool Bluetoothh::isConnected(){
	int i;
	for(i=4;i<8;i++){
		if (getUnpack(i)) return true;
	}
	return false;
}
