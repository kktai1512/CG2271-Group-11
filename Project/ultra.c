//#include "MKL25Z4.h"
//#define MASK(x) (1 << (x))
//// SENSOR
//#define ECHO_PIN 13 // PortA Pin 13
//#define TRIG_PIN 6 // PortD Pin 6

//#define SPEED_OF_SOUND 0.0343 // 343 METRES PER SEC

//#define uS(x)									(48 * x)
//#define mS(x)									(48000 * x)

//volatile float distance = 0.0;
//volatile int active = 0;
//volatile uint8_t sensorCount = 0;

///*----------------------------------------------------------------------------
// * Functions
// *---------------------------------------------------------------------------*/

//void initTimer(void) {
//	
//	// Enable Clock Gating For Timer 1
//	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
//	
//	//Select Clock for TPM module
//	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
//	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
//	
//	TPM1->MOD = 17480 - 1;
//	
//	// Update SnC register: CMOD = 01, PS = 100 (16)
//	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
//	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(4));
//	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
//	TPM1->SC &= ~(TPM_SC_TOIE_MASK); //Clear Overflow Interrupt Register
//	
//	
//	TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
//	//TPM1_C1SC |= TPM_CnSC_ELSA(1) | TPM_CnSC_ELSB(1);;
//	TPM1_C1SC |= TPM_CnSC_CHIE(1);
//	
//	
//	NVIC_EnableIRQ(TPM1_IRQn);
//	__enable_irq();
//}

//static void detectEcho(void) {
//	TPM1_C1SC |= TPM_CnSC_ELSA(1); //Rising Edge Input Capture
//	TPM1_C1SC |= TPM_CnSC_CHIE(1); //Enable Interrupt
//}

//static void startTimer(void) {
//	TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK);
//	TPM1_C1SC |= TPM_CnSC_ELSB(1); // Falling Edge interrupt
//	TPM1->CNT = 0;
//	TPM1->SC |= TPM_SC_TOIE(1); //Enable Overflow Interrupt
//	TPM1->SC |= TPM_SC_TOF(1);
//}

//static void resetTimer(void) {
//	TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK);
//	TPM1_C1SC &= ~(TPM_CnSC_CHIE_MASK); //Disable Channel Interrupt
//	TPM1->SC &= ~(TPM_SC_TOIE_MASK); // Disable Overflow Interrupt
//	TPM1->CNT = 0;
//}

//static void initUltrasonic(void) {
//	
//	//Enable clock gating for portA and portD
//	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
//	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
//	
//	PORTD->PCR[TRIG_PIN] &= ~PORT_PCR_MUX_MASK;
//	PORTD->PCR[TRIG_PIN] |= PORT_PCR_MUX(1);
//	
//	PORTA->PCR[ECHO_PIN] &= ~PORT_PCR_MUX_MASK;
//	PORTA->PCR[ECHO_PIN] |= PORT_PCR_MUX(3);
//	
//	PORTA->PCR[ECHO_PIN] &= ~PORT_PCR_PE_MASK;
//	
//	
//	PTD->PDDR |= MASK(TRIG_PIN);	//Set trig to output
//	PTA->PDDR &= ~MASK(ECHO_PIN); //Set echo to input
//}

//void delay(volatile uint32_t nof) {
//	while (nof != 0) {
//		__asm("NOP");
//		nof--;
//	}
//}

//static void pulse(void) {
//	PTD->PSOR |= MASK(TRIG_PIN);
//	detectEcho();
//	delay(uS(10));
//	
//	PTD->PCOR |= MASK(TRIG_PIN);
//}

//void TPM1_IRQHandler(void) {
//	NVIC_ClearPendingIRQ(TPM1_IRQn);
//	
//	if (TPM1_C1SC & TPM_CnSC_CHF_MASK) {
//		TPM1_C1SC |= TPM_CnSC_CHF(1); // If CHF = channel is 1 when an event occur- cleared by writing 1 
//		if (active) {
//			active = 0;
//			float multiplier = (343000.0 * 16)/48000000;
//			distance = ((TPM1_C1V) * multiplier)/2;
//			resetTimer();
//		}
//		else {
//			active = 1;
//			startTimer();
//		}
//	}
//	else if (TPM1->SC & TPM_SC_TOF_MASK) {
//		TPM1->SC |= TPM_SC_TOF(1); // Checking for overflow so that it resets when hit and counter = 1 cycle.
//		if (active) {
//			active = 0;
//			distance = 1000;
//			resetTimer();
//		}
//	}
//}

//int main (void) {
//    initUltrasonic();
//    initTimer();
//    while(1) {
//      pulse();
//			delay(0x20000);
//    }
//}
