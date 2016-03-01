#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "Motion.h"

/*
 * NavigatorPosition represents all possible field positions next to
 * supply rods.
 */
enum NavigatorPosition {
  REACTOR_A = 0,
  REACTOR_B = 1,
  SPENT_ROD_1 = 2,
  SPENT_ROD_2 = 3,
  SPENT_ROD_3 = 4,
  SPENT_ROD_4 = 5,
  NEW_ROD_1 = 6,
  NEW_ROD_2 = 7,
  NEW_ROD_3 = 8,
  NEW_ROD_4 = 9
};

/*
 * Navigator builds motion plans to navigate from one NavigatorPosition
 * to another. Plans are stored in a command buffer which can be executed
 * by execute plan. Navigator is non-blocking, and update() should be
 * called repeatedly.
 */
class Navigator {
public:
  Navigator(Motion *motion);
  int buildPlan(enum NavigatorPosition newPosition, char commandBuffer[]);
  void executePlan(char commandBuffer[], int count);
  void update();
  bool isDone();
  enum NavigatorPosition getIndex(); //Added by Dorothy

private:
  bool isNewRod(enum NavigatorPosition position);
  bool isSpentRod(enum NavigatorPosition position);
  bool isReactor(enum NavigatorPosition position);
  int fieldIndex(enum NavigatorPosition position);

  Motion *motion;
  enum NavigatorPosition position;
  char *commandBuffer;
  int count, index;
  bool done = true;
};

#endif
