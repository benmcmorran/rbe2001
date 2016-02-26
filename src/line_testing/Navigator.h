#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "Motion.h"

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

class Navigator {
public:
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
