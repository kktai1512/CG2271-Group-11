#include "motors.h"

int mod_val = 7500;

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
	TPM1_C0V = mod_val * percent/100;
	TPM1_C1V = 0;
}

void rearBackward(int percent) {
	TPM1_C1V = mod_val * percent/100;
	TPM1_C0V = 0;
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
	TPM1_C0V = 0;
	TPM1_C1V = 0;
}

/*intiPWM() */
void initMotorPWM(void)
{
	//enable clock
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) |
	 (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTE_MASK));

	//enable for m3
	//clear mux, set mux mode to alt. 3 (TPMCh0)
	PORTB->PCR[PTB0] &= ~PORT_PCR_MUX_MASK;

	PORTB->PCR[PTB0] |= PORT_PCR_MUX(3);

	//clear mux, set mux mode to alt. 3 (TPMCh1)

	PORTB->PCR[PTB1] &= ~PORT_PCR_MUX_MASK;

	PORTB->PCR[PTB1] |= PORT_PCR_MUX(3);

	//enable for m4 
	PORTE->PCR[PTE20] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE20] |= PORT_PCR_MUX(3);
	PORTE->PCR[PTE21] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE21] |= PORT_PCR_MUX(3);

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

	//timer 1 configurations
	//7500 for 50hz
	TPM1->MOD = mod_val;

	//clock mode and prescaler
	TPM1->SC &= ~((TPM_SC_CMOD_MASK)|(TPM_SC_PS_MASK));

	//LPTPM increments on every counter clock, prescaler = 128
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

	//center-aligned, up counting mode
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);

	//edge_or_level_select, channel mode select
	TPM1_C0SC &= ~ ((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	TPM1_C1SC &= ~ ((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}



static void delay(volatile uint32_t nof)
{
	while(nof!=0)
	{
		__asm("NOP");
		nof--;
	}
}

void move(uint8_t rx_data) {
	switch(rx_data) {
		case 0b0000:
			stop();
			break;
		case 0b0001:
			right(20);
			break;
		case 0b0010:
			left(20);
			break;
		case 0b0011:
			forward(70);
			break;
		case 0b0100:
			backward(20);
			break;
		case 0b111:
			selfdrive();
	}
}
void selfdrive(void) {
	      stop();
        osDelay(0x500);
	
        left(100);
        osDelay(0x500);
	
        stop();
        osDelay(0x500);
	
        forward(100);
        osDelay(0x400);
				
        stop();
        osDelay(0x500);
	
        right(100);
        osDelay(0x875);
	
        stop();
        osDelay(0x500);
	
        forward(100);
        osDelay(0x400);
				
        stop();
        osDelay(0x500);
				
        right(100);
        osDelay(0x875);
				
        stop();
        osDelay(0x500);
				
        forward(100);
        osDelay(0x400);
				
        stop();
        osDelay(0x500);
				
        right(100);
        osDelay(0x800);
				
        stop();
        osDelay(0x500);
							
        forward(100);
        osDelay(0x400);
				
        stop();
        osDelay(0x500);
				
				left(100);
				osDelay(0x500);
				
				stop();
				osDelay(0x500);
				
        //forward(100);
        osDelay(0x700);
				
        stop();
        osDelay(0xFFFFFFFF);
}

void circular(int delay){
	rearForward(30);
	right(30);
	osDelay(delay);
	stop();
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