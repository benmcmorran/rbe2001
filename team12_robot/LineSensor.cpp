#include "Arduino.h"
#include "LineSensor.h"

#define NUM_LINE_SENSORS 8

/*
 * Constructs a new LineSensor. pins points to an array of 8
 * analog pin numbers, ordered from left to right across the
 * sensor. A referene to the array is not held outside the
 * constructor.
 */
LineSensor::LineSensor(const int pins[]) {
	for (int i = 0; i < NUM_LINE_SENSORS; i++)
		this->pins[i] = pins[i];
}

/*
 * Reads and buffers every sensor in the array. This function
 * should be called periodically.
 */
void LineSensor::update() {
	for (int i = 0; i < NUM_LINE_SENSORS; i++)
		readings[i] = analogRead(pins[i]);
}

/*
 * Returns the maximum (darkest) reading across the 8 sensors.
 */
int LineSensor::maxReading() {
  int maximum = 0;
	for (int i = 0; i < NUM_LINE_SENSORS; i++)
		if (readings[i] > maximum) maximum = readings[i];
	return maximum;
}

/*
 * Returns the average reading of the 8 sensors.
 */
int LineSensor::averageReading() {
  int sum = 0;
  for (int i = 0; i < NUM_LINE_SENSORS; i++)
    sum += readings[i];
  return sum / NUM_LINE_SENSORS;
}

/*
 * Computes the centroid of the readings and returns a value from -1
 * to 1, with -1 being full left and 1 being full right. The centroid
 * is useful because it takes advantage of the analog nature of the
 * readings and is robust to changes in lighting conditions.
 */
float LineSensor::averageLinePosition() {
	// readingSum is just a sum, while positionSum is weighted by position
	int readingSum = 0;
	float positionSum = 0.0;
	for (int i = 0; i < NUM_LINE_SENSORS; i++) {
		readingSum += readings[i];
    // Subtract 1 from NUM_LINE_SENSORS to account for the fencepost problem
    // Multiply by 2 and subtract 1 to map the 0-1 range to the -1-1 range
    positionSum += readings[i] * (((float)i / (NUM_LINE_SENSORS - 1)) * 2.0 - 1.0);
	}
	return positionSum / readingSum;
}

/*
 * Returns true if the currently sensor readings plausibly resemble a line.
 * A line is detected if at least one and no more than two contiguous
 * sensors read higher than 100. This is useful for ignoring strage results
 * at intersections.
 */
bool LineSensor::isLineDetected() {
  bool foundLine = false;
  for (int i = 0; i < NUM_LINE_SENSORS; i++) {
    if (readings[i] > 100) {
      if (!foundLine) {
        i++; // Skip the next reading
        foundLine = true;
      } else return false;
    }
  }
  return foundLine;
}

