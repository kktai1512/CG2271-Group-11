#include "basicConfig.h"

#define NUM_GREEN 8
#define NUM_RED 1
#define NUM_LED (NUM_GREEN + NUM_RED)
#define RED 12

void shiftGreenLED(void);
void InitLED(void);
void toggleLED(void);
void toggleRedLED();
void setGreenLED(int set);
void offRedLED();