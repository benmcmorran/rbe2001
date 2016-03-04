#include "Navigator.h"

/*
 * Constructs a new Navigator. A Motion pointer is needed to actually drive.
 */
Navigator::Navigator(Motion *motion) :
  motion(motion),
  // Assume we start at Reactor A
  position(REACTOR_A),
  commandBuffer(0),
  count(0),
  index(0)
  { }

/*
 * Creates a plan for navigation from the current position to newPosition.
 * The plan is stored in commandBuffer and buildPlan() returns the new length
 * of commandBuffer.
 */
int Navigator::buildPlan(enum NavigatorPosition newPosition, char commandBuffer[]) {
  // Questionable, but it should work for our purposes
  enum NavigatorPosition currentPosition = position;
  position = newPosition;
  int i = 0;
  
  // Nothing to do if we're already there
  if (newPosition == currentPosition) return i;
  
  // Spin around to face the right way
  commandBuffer[i++] = (char)BACKWARDS;
  commandBuffer[i++] = (char)TURN_180;

  // Get relative positions of the source and destination
  int newIndex = fieldIndex(newPosition);
  int oldIndex = fieldIndex(currentPosition);

  // If we're already aligned or going between reactors, then just line follow
  if (newIndex == oldIndex || (isReactor(currentPosition) && isReactor(newPosition))) {
    commandBuffer[i++] = (char)TRACK_TO_BUMP;
    return i;
  }

  // If we're currently on the sides then we need to go to the main line
  if (isNewRod(currentPosition) || isSpentRod(currentPosition)) {
    commandBuffer[i++] = (char)TRACK_TO_INTERSECTION;
    commandBuffer[i++] = (char)1;
  }

  // Compute the right way to turn based on the destination and which
  // side of the field we came from
  if (isNewRod(currentPosition)) {
    commandBuffer[i++] = newIndex - oldIndex > 0 ? (char)TURN_RIGHT : (char)TURN_LEFT;
  } else if (isSpentRod(currentPosition)) {
    commandBuffer[i++] = newIndex - oldIndex > 0 ? (char)TURN_LEFT : (char)TURN_RIGHT;
  }

  // Once on the center line, the reactor is just a TRACK_TO_BUMP away
  if (isReactor(newPosition)) {
    commandBuffer[i++] = (char)TRACK_TO_BUMP;
    return i;
  }

  // Otherwise, we need to track some number of intersections to the next fuel rod
  commandBuffer[i++] = (char)TRACK_TO_INTERSECTION;
  commandBuffer[i++] = (char)abs(newIndex - oldIndex);
  
  // Compute the right direction to turn again
  if ((isSpentRod(newPosition) && newIndex - oldIndex > 0) || (isNewRod(newPosition) && newIndex - oldIndex < 0)) {
    commandBuffer[i++] = (char)TURN_LEFT;
  } else {
    commandBuffer[i++] = (char)TURN_RIGHT;
  }

  // Track all the way to the wall
  commandBuffer[i++] = (char)TRACK_TO_BUMP;

  // Return the number of chars written to the buffer
  return i;
}

/*
 * Configures the Navigator to execute the given plan. update() must be called
 * to actually move.
 */
void Navigator::executePlan(char commandBuffer[], int count) {
  this->commandBuffer = commandBuffer;
  this->count = count;
  index = -1; // Start at -1 so that incrementing in the first update will set it to 0
  done = false;
}

/*
 * Moves the robot according to the computed plan. This function must
 * be called periodically.
 */
void Navigator::update() {
  // Nothing to do if we're done
  if (done) return;

  // Updates the robot's motion
  motion->update();

  // Check if the current action is finished
  if (motion->isDone()) {
    index++;

    // Are we done with the whole plan?
    if (index == count) {
      done = true;
      return;
    }

    // Parse and start the next instruction
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
      // TRACK_TO_INTERSECTION takes up two bytes instead of one
      case TRACK_TO_INTERSECTION:
        // Get the next byte to know how many intersections to count
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

/*
 * Returns true if the Navigator is done executing the current plan.
 */
bool Navigator::isDone() {
  return done;
}

/*
 * Returns true if the given position represents a new rod.
 */
bool Navigator::isNewRod(enum NavigatorPosition position) {
  return NEW_ROD_1 <= position && position <= NEW_ROD_4;
}

/*
 * Returns true if the given position represents a spent rod.
 */
bool Navigator::isSpentRod(enum NavigatorPosition position) {
  return SPENT_ROD_1 <= position && position <= SPENT_ROD_4;
}

/*
 * Returns true if the given position represents a reactor.
 */
bool Navigator::isReactor(enum NavigatorPosition position) {
  return position == REACTOR_A || position == REACTOR_B;
}

/*
 * Returns the relative left-right position of the given
 * position on the field. REACTOR_A is 0 and REACTOR_B is 5.
 */
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


/*
 * Returns the current position of the Navigator.
 */
//Added by Dorothy
enum NavigatorPosition Navigator::getIndex(){
  return position;
}
