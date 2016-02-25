#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "Motion.h"

enum NavigatorPosition {
  REACTOR_A,
  REACTOR_B,
  SPENT_ROD_1,
  SPENT_ROD_2,
  SPENT_ROD_3,
  SPENT_ROD_4,
  NEW_ROD_1,
  NEW_ROD_2,
  NEW_ROD_3,
  NEW_ROD_4
};

class Navigator {
  Navigator(Motion *motion);
  int buildPlan(enum NavigatorPosition newPosition, char commandBuffer[]);
  void executePlan(char commandBuffer[], int count);
  void update();
  
private:
  bool isNewRod(enum NavigatorPosition position);
  bool isSpentRod(enum NavigatorPosition position);
  bool isReactor(enum NavigatorPosition position);
  int fieldIndex(enum NavigatorPosition position);

  Motion *motion;
  enum NavigatorPosition position;
  char *commandBuffer;
  int count, index;
};

#endif
