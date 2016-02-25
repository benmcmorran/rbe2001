#include "Navigator.h"

Navigator::Navigator(Motion *motion) :
  motion(motion),
  position(REACTOR_A),
  commandBuffer(0),
  count(0),
  index(0)
  { }

int Navigator::buildPlan(enum NavigatorPosition newPosition, char commandBuffer[]) {
  int i = 0;

  if (newPosition == position) return i;
  
  commandBuffer[i++] = (char)BACKWARDS;
  commandBuffer[i++] = (char)TURN_180;

  int newIndex = fieldIndex(newPosition);
  int oldIndex = fieldIndex(position);

  if (newIndex == oldIndex) {
    commandBuffer[i++] = (char)TRACK_TO_BUMP;
    return i;
  }

  if (isNewRod(position)) {
    commandBuffer[i++] = newIndex - oldIndex > 0 ? (char)TURN_RIGHT : (char)TURN_LEFT;
  } else if (isSpentRod(position)) {
    commandBuffer[i++] = newIndex - oldIndex > 0 ? (char)TURN_LEFT : (char)TURN_RIGHT;
  }

  return i;
}

void executePlan(char commandBuffer[], int count);
void update();

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
    REACTOR_A: return 0;
    REACTOR_B: return 5;
    SPENT_ROD_1: return 4;
    SPENT_ROD_2: return 3;
    SPENT_ROD_3: return 2;
    SPENT_ROD_4: return 1;
    NEW_ROD_1: return 1;
    NEW_ROD_2: return 2;
    NEW_ROD_3: return 3;
    NEW_ROD_4: return 4;
  }
}

