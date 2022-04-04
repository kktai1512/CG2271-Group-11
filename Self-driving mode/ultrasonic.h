#include "basicConfig.h"
#define MASK(x) (1 << (x))

#define ECHO_IN 1 // PortA Pin 1
#define TRIG_OUT 4 // PortD Pin 4

#define SPEED_OF_SOUND 343000.0 // 343 m/s
#define PRESCALER 16

#define uS(x)									(48 * x)
#define mS(x)									(48000 * x)

void initTimer(void);
void resetTimer(void);
void initUltrasonic(void); 
void pulse(void);
void TPM2_IRQHandler(void);

