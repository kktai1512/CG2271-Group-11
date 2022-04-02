#include "MKL25Z4.h"
#define MASK(x) (1 << (x))

#define ECHO_IN 1 // PortA Pin 13
#define TRIG_OUT 4 // PortD Pin 4

#define SPEED_OF_SOUND 343000.0 // 343 cm/s
#define PRESCALER 16

#define uS(x)									(48 * x)
#define mS(x)									(48000 * x)

volatile float distance = 0.0;
volatile int active = 0;
volatile uint8_t sensorCount = 0;

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


static void resetTimer(void) {
	TPM2->CNT = 0;
}

static void initUltrasonic(void) {

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

static void pulse(void) {
	PTD->PSOR |= MASK(TRIG_OUT);
	//detectEcho();
	delay(uS(10));
	
	PTD->PCOR |= MASK(TRIG_OUT);
}

void TPM2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	
	if (TPM2_C0SC & TPM_CnSC_CHF_MASK) {
		TPM2_C0SC |= TPM_CnSC_CHF(1); // If CHF = channel is 1 when an event occur- cleared by writing 1 
		if (active) {
			active = 0;
			float multiplier = (SPEED_OF_SOUND * PRESCALER)/48000000;
			distance = ((TPM2_C0V) * multiplier)/2;
			resetTimer();
		}
		else {
			active = 1;
			resetTimer();
		}
	}
	else if (TPM2->SC & TPM_SC_TOF_MASK) {
		TPM2->SC |= TPM_SC_TOF(1); // Checking for overflow so that it resets when hit and counter = 1 cycle.
		if (active) {
			active = 0;
			distance = 1000;
			resetTimer();
		}
	}
}

int main (void) {
    initUltrasonic();
    initTimer();
    while(1) {
      pulse();
			delay(0x20000);
    }
}
