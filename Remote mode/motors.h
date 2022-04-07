#include "basicConfig.h"
//M3 AND M4 ARE REAR
#define PTB0 0 //m3


//M1 AND M2 ARE FRONT
#define PTD1 1 //	m2
#define PTD3 3 //m2
#define PTD2 2 //m1
#define PTD0 0 //m1

#define STOP 0
#define RIGHT 1
#define LEFT 2
#define FORWARD 3
#define REVERSE 4
#define F_RIGHT 5
#define F_LEFT 6
#define END 10

void m1_forward(int percent);
void m1_backward(int percent);
void m2_forward(int percent);
void m2_backward(int percent);
void left(int percent);
void right(int percent);
void frontForward(int percent);
void frontBackward(int percent);
void rearForward(int percent);
void rearBackward(int percent);
void forward(int percent);
void backward(int percent);
void initMotorPWM(void);
void stop();
void move(uint8_t);
void square();
