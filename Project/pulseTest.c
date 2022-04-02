//#include "MKL25Z4.h"
//#define ECHO_IN 1 //PORTA_1 Timer 2 C0
//#define TRIG_OUT 5 //PORTD_4 GPIO
//#define MASK(x) (1 << (x))

//static void initGPIO(void) {

//    //enable clock for echo and trig
//    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
//    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

//    //set port a to timer mode (input capture)
//    PORTA->PCR[ECHO_IN] &= ~PORT_PCR_MUX_MASK;
//    PORTA->PCR[ECHO_IN] |= PORT_PCR_MUX(3);

//    //set to GPIO mode
//    PORTD->PCR[TRIG_OUT] &= ~PORT_PCR_MUX_MASK;
//    PORTD->PCR[TRIG_OUT] |= PORT_PCR_MUX(1);

//    PTA->PDDR &= ~MASK(ECHO_IN);
//    PTD->PDDR |= MASK(TRIG_OUT);
//}

//static void delay (volatile uint32_t nof) {
//    while (nof!=0) {
//    __asm ("NOP");
//    nof--;
//    }
//}
//void sendPulse(void) {
//    PTD->PSOR |= MASK(TRIG_OUT);
//    delay(0x80000);
//    PTD->PCOR |= MASK(TRIG_OUT);
//}
//int main (void) {
//    initGPIO();
//    while(1) {
//			sendPulse();
//    }
//		
//}