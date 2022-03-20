#include "MKL25Z4.h"    
#include<stdio.h>
#include "Buzzer.h"


int main(void)
{
	SystemCoreClockUpdate();
	
	initPWM();		
		
	int size=arrayLength(score_2);
	
	while(1) //output each frequency at fixed intervals
	{			
		play_song(size,score_2,notes);
  }
}


