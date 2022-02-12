#include<stdio.h>

enum color {red,blue,green};

#define RED_LED			18  //portb pin 18
#define GREEN_LED		19	//portb pin 19
#define BLUE_LED 		1		//port d pin 1
#define SW_POS			6
#define MASK(x) 		(1<< (x))
#include "MKL25Z4.h"                    // Device header
#define PTB0_Pin 0
#define PTB1_Pin 1


//prescaler of 256 for TPM_SC_PS(7)
int freq_to_mod(float freq, float pre) {
    float mod;
    mod = (48000000) / pre * (1 / freq);
    mod = (int)mod;
    return mod;
}
	

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
	
	
	// mod_val not needed in here
	
	//edge aligned pwm
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
									
	
}

int notes[]={262,294,330,349,392,440,494}; //C,D,E,F,G,A,B

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
	
	while(1) //output each frequency at fixed intervals
	{			
		for (int i; i<7;i++){
			TPM1_C0V = freq_to_mod(notes[i],256)/2;
			TPM1->MOD = freq_to_mod(notes[i],256);
			delay(0x80000);
	  }
  }
}
