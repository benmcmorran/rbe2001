#ifndef ROBOTT_H
#define ROBOTT_H


class Robott{
	public:
		Robott();
		void mainloop();
		void navigate_to_new_rod(); //state1
		void navigate_to_spent_rod(); //state2
		void navigate_to_empty_space(); //state3
		void get_rod(); //state4
		void store_rod(); //state5
		void line_following(); //helper func


	private:
		Bluetoothh blueth;
		Motor mt;
		LineSensor lsr;
		//need a roller obj
		bool isHB;
		int state; 


};





#endif