#include<stdio.h>
volatile unsigned int counter=0;

enum color {red,blue,green};

#define RED_LED			18  //portb pin 18
#define GREEN_LED		19	//portb pin 19
#define BLUE_LED 		1		//port d pin 1
#define SW_POS			6
#define MASK(x) 		(1<< (x))
#include "MKL25Z4.h"                    // Device header
#define PTB0_Pin 0
#define PTB1_Pin 1

int mod_val = 15000;

void initPWM()
{
	//eNABLE CLOCK FOR PORTB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
	
	//enable clock for timer1
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	//select clock for tpm module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	
	//SHOULD BE THE MOD VAL FOR 50Hz
	TPM1->MOD = mod_val;
	
	//edge aligned pwm
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
									
	
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
	
	initPWM();
	//TPM1_C0V = 0x0EA6;
	//TPM1_C0V = 1875;
	TPM1_C0V = mod_val/2;
	while(1)
	{			
	}
}

