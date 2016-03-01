#ifndef LineSensor_h
#define LineSensor_h

#include "Arduino.h"

/*
 * Encapsulates an 8 analog line sensor array and provides 
 * useful functions for reading it.
 */
class LineSensor {
public:
	LineSensor(const int pins[]);
	int maxReading();
  int averageReading();
	float averageLinePosition();
  bool isLineDetected();
	void update();
private:
	int pins[8];
	int readings[8];
};

#endif
