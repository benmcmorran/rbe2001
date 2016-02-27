#include "Navigator.h"

Navigator::Navigator(Motion *motion) :
  motion(motion),
  position(REACTOR_A),
  commandBuffer(0),
  count(0),
  index(0)
  { }

int Navigator::buildPlan(enum NavigatorPosition newPosition, char commandBuffer[]) {
  // Questionable, but it should work for our purposes
  enum NavigatorPosition currentPosition = position;
  position = newPosition;
  int i = 0;
  
  if (newPosition == currentPosition) return i;
  
  commandBuffer[i++] = (char)BACKWARDS;
  commandBuffer[i++] = (char)TURN_180;

  int newIndex = fieldIndex(newPosition);
  int oldIndex = fieldIndex(currentPosition);

  if (newIndex == oldIndex || (isReactor(currentPosition) && isReactor(newPosition))) {
    commandBuffer[i++] = (char)TRACK_TO_BUMP;
    return i;
  }

  if (isNewRod(currentPosition) || isSpentRod(currentPosition)) {
    commandBuffer[i++] = (char)TRACK_TO_INTERSECTION;
    commandBuffer[i++] = (char)1;
  }

  if (isNewRod(currentPosition)) {
    commandBuffer[i++] = newIndex - oldIndex > 0 ? (char)TURN_RIGHT : (char)TURN_LEFT;
  } else if (isSpentRod(currentPosition)) {
    commandBuffer[i++] = newIndex - oldIndex > 0 ? (char)TURN_LEFT : (char)TURN_RIGHT;
  }

  if (isReactor(newPosition)) {
    commandBuffer[i++] = (char)TRACK_TO_BUMP;
    return i;
  }

  commandBuffer[i++] = (char)TRACK_TO_INTERSECTION;
  commandBuffer[i++] = (char)abs(newIndex - oldIndex);
  
  if ((isSpentRod(newPosition) && newIndex - oldIndex > 0) || (isNewRod(newPosition) && newIndex - oldIndex < 0)) {
    commandBuffer[i++] = (char)TURN_LEFT;
  } else {
    commandBuffer[i++] = (char)TURN_RIGHT;
  }

  commandBuffer[i++] = (char)TRACK_TO_BUMP;

  return i;
}

void Navigator::executePlan(char commandBuffer[], int count) {
  this->commandBuffer = commandBuffer;
  this->count = count;
  index = -1; // Start at -1 so that incrementing in the first update will set it to 0
  done = false;
}

void Navigator::update() {
  if (done) return;

  motion->update();
  
  if (motion->isDone()) {
    index++;
    
    if (index == count) {
      done = true;
      return;
    }

    int num;
    switch ((enum MotionState)commandBuffer[index]) {
      case TURN_RIGHT:
        motion->turnRight();
        break;
      case TURN_LEFT:
        motion->turnLeft();
        break;
      case TURN_180:
        motion->turn180();
        break;
      case BACKWARDS:
        motion->reverse();
        break;
      case TRACK_TO_INTERSECTION:
        index++;
        num = commandBuffer[index];
        motion->trackToIntersection(num);
        break;
      case TRACK_TO_BUMP:
        motion->trackToBump();
        break;
    }
  }
}

bool Navigator::isDone() {
  return done;
}

bool Navigator::isNewRod(enum NavigatorPosition position) {
  return NEW_ROD_1 <= position && position <= NEW_ROD_4;
}

bool Navigator::isSpentRod(enum NavigatorPosition position) {
  return SPENT_ROD_1 <= position && position <= SPENT_ROD_4;
}

bool Navigator::isReactor(enum NavigatorPosition position) {
  return position == REACTOR_A || position == REACTOR_B;
}

int Navigator::fieldIndex(enum NavigatorPosition position) {
  switch (position) {
    case REACTOR_A: return 0;
    case REACTOR_B: return 5;
    case SPENT_ROD_1: return 4;
    case SPENT_ROD_2: return 3;
    case SPENT_ROD_3: return 2;
    case SPENT_ROD_4: return 1;
    case NEW_ROD_1: return 1;
    case NEW_ROD_2: return 2;
    case NEW_ROD_3: return 3;
    case NEW_ROD_4: return 4;
  }
}

