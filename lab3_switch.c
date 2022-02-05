//#include "MKL25Z4.h"                    // Device header
//#include "lab2.h"
#include<stdio.h>
volatile unsigned int counter=0;

enum color {red,blue,green};

#define RED_LED			18  //portb pin 18
#define GREEN_LED		19	//portb pin 19
#define BLUE_LED 		1		//port d pin 1
#define SW_POS			6
#define MASK(x) 		(1<< (x))
#include "MKL25Z4.h"                    // Device header
//#include "lab2.h"

void InitGPIO(void)
{
	//enable clock to portb and portd
	//we need to do this because we are using the ports 
	//and we need to enable clock to turn it on 
	
	//SYSTEM CLOCK GATING CONTROL REGISTER 5 CONTAINS THE PORTS WE NEED 
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	//these variables are defined in the header file that we included
	
	//next we need to confiigure multilplexer settings to make these pins gpio
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK; //CLEAR EXISTING
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1); //SET 
	
	//NOW WE NEED TO DO THE SAME FOR GREEN LED UNDER PORTB
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	
	//THE 1 THAT WE ARE SETTING REFERS TO US USING THE PORT FOR GPIO
	//ONE STANDS FOR ALTERNATIVE- GPIO
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	//NEXT WE NEED TO SET THE DATA DIRECTION REGISTERS FOR PORTB AND D 
	//WE NEED TO SET AS OUTPUT, SO WE WILL BE SETTING PDDR AS 1
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
	
	//HERE WE ARE USING OR= BECAUSE WE NEED TO SET AS 1 - OUTPUT
	//ELSE WE WILL BE using AND=
	
	//after initializing we turn off all leds
	PTB->PDOR = (MASK(GREEN_LED)|MASK(RED_LED));
	PTD->PDOR = MASK(BLUE_LED);
}


//now i need to write my own functions to turn on the led 
//void led_control(color_t color)
//enum color {red,blue,green};
volatile char led_control =1;

void led_on(enum color x)
{
	//since the leds are active low we need to 
	//clear the bit inside to turn it on
	switch(x)
	{
		case red:
			PTB->PCOR |= MASK(RED_LED);
			break;
		case green:
			PTB->PCOR |= MASK(GREEN_LED);
			break;
		case blue:
			PTD->PCOR |= MASK(BLUE_LED);
			break;
	}
			
	//
}

void off_rgb()
{
	PTB->PDOR = (MASK(GREEN_LED)|MASK(RED_LED));
	PTD->PDOR = MASK(BLUE_LED);
}

//LAB3 

void initSwitch(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	PORTD->PCR[SW_POS] |= (PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a));
	
	PTD->PDDR &= ~MASK(SW_POS);
	
	NVIC_SetPriority(PORTD_IRQn,2);
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
	
}

void PORTD_IRQHandler()
{
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	
	//counter++;
	led_control^=1;
	
	PORTD->ISFR |= MASK(SW_POS);
}


static void delay(volatile uint32_t nof)
{
	while(nof!=0)
	{
		__asm("NOP");
		nof--;
	}
}
int main(void)
{
	SystemCoreClockUpdate();
	InitGPIO();
	initSwitch();
	int i = red;
	
	while(1)
	{
		

		if(led_control)
		{
		counter++;
		off_rgb();
		led_on(i);
		delay(0x80000);
		i++;

		if(i==green)
		{
			off_rgb();
			led_on(i);
			delay(0x80000);
			i = red;
		}
	
		
		if(counter> 0x0F)
		{
			counter=0;

		}
	}
		else off_rgb();
			
		
	}
}

