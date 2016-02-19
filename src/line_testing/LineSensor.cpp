#include "Arduino.h"
#include "LineSensor.h"

#define NUM_LINE_SENSORS 8

LineSensor::LineSensor(const int pins[]) {
	for (int i = 0; i < NUM_LINE_SENSORS; i++)
		this->pins[i] = pins[i];
}

void LineSensor::update() {
	for (int i = 0; i < NUM_LINE_SENSORS; i++)
		readings[i] = analogRead(pins[i]);
}

int LineSensor::maxReading() {
  int maximum = 0;
	for (int i = 0; i < NUM_LINE_SENSORS; i++)
		if (readings[i] > maximum) maximum = readings[i];
	return maximum;
}

int LineSensor::averageReading() {
  int sum = 0;
  for (int i = 0; i < NUM_LINE_SENSORS; i++)
    sum += readings[i];
  return sum / NUM_LINE_SENSORS;
}

float LineSensor::averageLinePosition() {
	int readingSum = 0;
	float positionSum = 0.0;
	for (int i = 0; i < NUM_LINE_SENSORS; i++) {
		readingSum += readings[i];
    positionSum += readings[i] * (((float)i / (NUM_LINE_SENSORS - 1)) * 2.0 - 1.0);
	}
	return positionSum / readingSum;
}

