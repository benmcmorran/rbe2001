#include "Bluetoothh.h"
#include <Arduino.h>

Bluetoothh::Bluetoothh(byte teamn):team(teamn),pcol(teamn)
{
}

/**Send a heart beat message
**/
void Bluetoothh::sendHB(){
	byte data[3];
	byte package[10];
	int size;

	pcol.setDst(0x00);//broadcast message
	size = pcol.createPkt(0x07, data, package); //creat a package with type heartbeat 
    btmaster.sendPkt(package, size); //sending
}


/**Send a low level alert message
**/
void Bluetoothh::sendLowAlert(){
	byte data[3];
	byte package[10];
	int size;

	data[0] = 0x2C; //low alert data
	pcol.setDst(0x00); //broadcast message
	size = pcol.createPkt(0x03, data, package); //creat a package with type alert
    btmaster.sendPkt(package, size); //sending
}

/**Send a high level alert message
**/
void Bluetoothh::sendHighAlert(){
	byte data[3];
	byte package[10];
	int size;

	data[0] = 0xFF;//high alert data
	pcol.setDst(0x00);//broadcast message
	size = pcol.createPkt(0x03, data, package);//creat a package with type alert
    btmaster.sendPkt(package, size); //sending
}

/**Checking bluetooth and store messages
**/
void Bluetoothh::checkstatus(){
	byte data[3];
	byte package[10];
	byte type;
	int size;
	if(btmaster.readPacket(package)){ //if successfully read a package
		if (pcol.getData(package, data, type)){ //get the data and the type of the message
			if(package[4]== 0x00||package[4]==team){ //if the message is broadcasting or sending to our robot
				switch(type){
					case 0x01: //storage availability
						message[0] = data[0]; //store the data to the zeroth element of a byte array called message
						break;
					case 0X02: //supply availability
						message[1] = data[0]; //store the data to the last element of a byte array called message
						break;
					case 0x04: //stop movement
						stopMoving = true; //set the stopMoving flag to true
						break;
					case 0x05: //resume movement
						stopMoving = false; //flip the stopMoving flag down
						break;
					default: //imaginery type
						break;
				}
			}
		}	
	}
	unpack(); //deciphering the messages
}

/**Unpacking the bluetooth messages the robot receives and store the decoded message into a boolean array
**/
void Bluetoothh::unpack(){		
		unpacked[3] = (bool) !((message[0] & (byte) 0x01)); //true if the fourth storage tube is empty
		unpacked[2] = (bool) !((message[0] & (byte) 0x02) >>1); //true if the third storage tube is empty
		unpacked[1] = (bool) !((message[0] & (byte) 0x04) >>2); //true if the second storage tube is empty
		unpacked[0] = (bool) !((message[0] & (byte) 0x08) >>3); //true if the first storage tube is empty
		unpacked[4] = (bool) (message[1] & 0x01); //true if the first new rod tube is filled
		unpacked[5] = (bool) ((message[1] & 0x02) >>1); //true if the second new rod tube is filled
		unpacked[6] = (bool) ((message[1] & 0x04) >>2); //true if the third new rod tube is filled
		unpacked[7] = (bool) ((message[1] & 0x08) >>3); //true if the fourth new rod tube is filled
}


/**Get one of the decoded message
	@param bluetoothIndex Index indicating the location of the tube
	@return Boolean indicating the empty availability of the spent field or the rod availability for the new field
**/
bool Bluetoothh::getUnpack(int bluetoothIndex){
	return unpacked[bluetoothIndex];
}

/**Sending robot status(We didn't really use it)
**/
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

/**Get the team number(for the debugging reason)
	@return team The team number in byte
**/
byte Bluetoothh::getTeam(){
  return team;
}

/**Check if the bluetooth is connected
**/
bool Bluetoothh::isConnected(){
	int i;
	for(i=4;i<8;i++){ //loop through the messages of new field
		if (getUnpack(i)) return true; //if any of the rod availabilty is updated, return true
	}
	return false; //if no message is updated, return false
}
