#include "motors.h"
int mod_val = 3750;

#define PTC8 8
#define PTC9 9

void m1_forward(int percent) {
	TPM0_C0V = mod_val * percent/100;
	TPM0_C2V = 0;
}

void m1_backward(int percent) {
	TPM0_C0V = 0; 
	TPM0_C2V = mod_val * percent/100;
}

void m2_forward(int percent) {
	TPM0_C1V = mod_val * percent/100;
	TPM0_C3V = 0;
}

void m2_backward(int percent) {
	TPM0_C1V = 0; 
	TPM0_C3V = mod_val * percent/100;
}

void left(int percent) {
	m1_backward(percent);
	m2_forward(percent);
}

void right(int percent) {
	m2_backward(percent);
	m1_forward(percent);
}

void frontForward(int percent) {
	m1_forward(percent);
	m2_forward(percent);
}

void frontBackward(int percent) {
	m1_backward(percent);
	m2_backward(percent);
}

void rearForward(int percent) {
	TPM0_C5V = mod_val * percent/100;
	TPM0_C4V = 0;
}

void rearBackward(int percent) {
	TPM0_C4V = mod_val * percent/100;
	TPM0_C5V = 0;
}

void forward(int percent) {
	frontForward(percent);
	rearForward(percent);
}

void backward(int percent) {
	frontBackward(percent);
	rearBackward(percent);
}

void stop() {
	TPM0_C0V = 0;
	TPM0_C1V = 0;
	TPM0_C2V = 0;
	TPM0_C3V = 0;
	TPM0_C4V = 0;
	TPM0_C5V = 0;
}

/*intiPWM() */
void initMotorPWM(void)
{
	//enable clock
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) |
	 (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTE_MASK) | (SIM_SCGC5_PORTC_MASK));


	//enable for m4 
	PORTC->PCR[PTC8] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[PTC8] |= PORT_PCR_MUX(3);
	
	PORTC->PCR[PTC9] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[PTC9] |= PORT_PCR_MUX(3);

	//enable for m2
	PORTD->PCR[PTD1] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD1] |= PORT_PCR_MUX(4);
	PORTD->PCR[PTD3] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD3] |= PORT_PCR_MUX(4);

	//enable for m1
	PORTD->PCR[PTD2] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD2] |= PORT_PCR_MUX(4);
	PORTD->PCR[PTD0] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[PTD0] |= PORT_PCR_MUX(4);

	//enable clock for timer 0 and timer 1
	SIM->SCGC6 |= ((SIM_SCGC6_TPM1_MASK) | (SIM_SCGC6_TPM0_MASK));

	//clear clock source & set clock source to MCGFLLCLK clock
	SIM->SOPT2 &= ~ SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	//timer 0 configuration
	TPM0->MOD = mod_val;
	TPM0->SC &= ~((TPM_SC_CMOD_MASK)|(TPM_SC_PS_MASK));
	TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	TPM0_C0SC &= ~ ((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM0_C1SC &= ~ ((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM0_C2SC &= ~ ((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM0_C2SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM0_C3SC &= ~ ((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM0_C3SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	TPM0_C4SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C4SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	TPM0_C5SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C5SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}




static void delay(volatile uint32_t nof)
{
	while(nof!=0)
	{
		__asm("NOP");
		nof--;
	}
}

void step() {
	right(100);
	osDelay(1950);
	
	stop();
	osDelay(1000);
	
	forward(50);
	osDelay(1200);
	
	stop();
	osDelay(1000);
}

 void lt() {
	left(100);
	osDelay(1950);
	
	stop();
	osDelay(1000);
	
	forward(50);
	osDelay(600);
	
	stop();
	osDelay(1000);
	
}


 void rt() {
	right(100);
	osDelay(1950);
	
	stop();
	osDelay(1000);
	
	forward(50);
	osDelay(600);
	
	stop();
	osDelay(1000);
	
}
void move(uint8_t rx_data) {
	switch(rx_data) {
		case 0b0000:
			stop();
			break;
		case 0b0001:
			right(100);
			break;
		case 0b0010:
			left(100);
			break;
		case 0b0011:
			forward(50);
			break;
		case 0b0100:
			backward(50);
			break;
		case 0b101:
			//swerve
			left(100);
			rearForward(40);
			break;
		case 0b110:
			//swerve
			right(100);
			rearForward(40);
			break;
		case 0b1010:
			osDelay(100);
			triangle();
			osDelay(10000000);
		case 0b1111:
			forward(25);
		case 0b1001:
			forward(100);
	}
}
void triangle() {
	sixty();
	onetwenty();
	onetwenty2();
	sixty();
	forward(50);
	osDelay(4000);
	stop();
}
void temp() {
	stop();
		osDelay(1000);
		lt();
		step();
		step();
		step();
		rt();
		lt();
}

void sixty() {
	rearForward(50);
	left(85);
	osDelay(650);
	
	stop();
	osDelay(300);
	
	forward(50);
	osDelay(1000);
	
	stop();
	osDelay(300);
}

void onetwenty() {
	rearForward(55);
	right(90);
	osDelay(1100);
	
	stop();
	osDelay(300);
	
	forward(50);
	osDelay(1600);
	
	stop();
	osDelay(300);
}

void onetwenty2() {
	rearForward(55);
	right(90);
	osDelay(1100);
	
	stop();
	osDelay(300);
	
	forward(50);
	osDelay(1300);
	
	stop();
	osDelay(300);
}

/*
int main(void)

{
	SystemCoreClockUpdate();
	initPWM();

	//TPM1_C0V = mod_val/2;
	while(1)
	{
		//lower channel goes to ain2
		//higher channel goes to ain1

		
		//m1_forward(20);
		rearForward(20);
		delay(0x80000 *2);
		rearBackward(20);
		//m2_backward(20);
		delay(0x80000 *2);
		continue;
		
	}
}
*/