#ifndef LineSensor_h
#define LineSensor_h

#include "Arduino.h"

class LineSensor {
public:
	LineSensor(const int pins[]);
	int maxReading();
  int averageReading();
	float averageLinePosition();
	void update();
private:
	int pins[8];
	int readings[8];
};

#endif
