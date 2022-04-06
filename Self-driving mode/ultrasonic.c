#include "ultrasonic.h"

/*----------------------------------------------------------------------------
* Functions
*---------------------------------------------------------------------------*/


void initTimer(void) {
	
	// Enable Clock Gating For Timer 1
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	//Select Clock for TPM module
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	TPM2->MOD = 17480 - 1;
	
	// Update SnC register: CMOD = 01, PS = 100 (16)
	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(4));
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
	TPM2->SC &= ~(TPM_SC_TOIE_MASK); //Clear Overflow Interrupt Register
	
	
	TPM2_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	
	// Start timer
	TPM2_C0SC |= (TPM_CnSC_ELSA(1) | TPM_CnSC_ELSB(1)); //Falling and Rising Edge Input Capture
	TPM2->CNT = 0;
	
	
	TPM2_C0SC |= TPM_CnSC_CHIE(1); // Enable Channel interrupt
	TPM2->SC |= TPM_SC_TOIE(1); //Enable Overflow Interrupt
	TPM2->SC |= TPM_SC_TOF(1); // Write 1 to clear
	

	NVIC_EnableIRQ(TPM2_IRQn);
	__enable_irq();	
}


void resetTimer(void) {
	TPM2->CNT = 0;
}

void initUltrasonic(void) {

   //enable clock for echo and trig
   SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
   SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

   //set port a to timer mode (input capture)
   PORTA->PCR[ECHO_IN] &= ~PORT_PCR_MUX_MASK;
   PORTA->PCR[ECHO_IN] |= PORT_PCR_MUX(3);

   //set to GPIO mode
   PORTD->PCR[TRIG_OUT] &= ~PORT_PCR_MUX_MASK;
   PORTD->PCR[TRIG_OUT] |= PORT_PCR_MUX(1);

   PTA->PDDR &= ~MASK(ECHO_IN);
   PTD->PDDR |= MASK(TRIG_OUT);
}

void delay(volatile uint32_t nof) {
	while (nof != 0) {
		__asm("NOP");
		nof--;
	}
}

void pulse(void) {
	PTD->PSOR |= MASK(TRIG_OUT);
	//detectEcho();
	delay(uS(10));
	
	PTD->PCOR |= MASK(TRIG_OUT);
}



/*
int main (void) {
   initUltrasonic();
   initTimer();
   while(1) {
     pulse();
			delay(0x20000);
   }
} */