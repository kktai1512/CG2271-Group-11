#ifndef buzzer_h
#define buzzer_h

#define SW_POS			6
#define MASK(x) 		(1<< (x))
#include "MKL25Z4.h"                    // Device header
#define PTC8_Pin 8
#define PTC9_Pin 9
#define arrayLength(array) (sizeof((array))/sizeof((array)[0]))

int notes[]={262,294,330,349,392,440,494,9999}; //C,D,E,F,G,A,B,blank

int score[]={0,7,0,7,4,7,4,7,5,7,5,7,4,7,7,7,3,7,3,7,2,7,2,7,1,7,1,7,0,7,7,7,
	4,7,4,7,3,7,3,7,2,7,2,7,1,7,7,7,4,7,4,7,3,7,3,7,2,7,2,7,1,7,7,7,
	0,7,0,7,4,7,4,7,5,7,5,7,4,7,7,7,3,7,3,7,2,7,2,7,1,7,1,7,0,7,7,7};
	
int score_2[]={2,7,2,7,3,7,4,7,4,7,3,7,2,7,1,7,0,7,0,7,1,7,2,7,2,7,1,7,1,7,7,
	2,7,2,7,3,7,4,7,4,7,3,7,2,7,1,7,0,7,0,7,1,7,2,7,1,7,0,7,0,7,7,
	1,7,1,7,2,7,0,7,1,7,3,7,2,7,1,7,0,7,1,7,3,7,2,7,1,7,0,7,1,7,4,7,7,
	2,7,2,7,3,7,4,7,4,7,3,7,2,7,1,7,0,7,0,7,1,7,2,7,1,7,0,7,0,7,7};	
	
	
int freq_to_mod(float freq, float pre);

void initPWM();

void play_song(int size, int arr[],int arr2[]);
	
static void delay(volatile uint32_t nof);	

#endif
