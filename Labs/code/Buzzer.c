#include "MKL25Z4.h"
#define PTB0_Pin 0
#define PTB1_Pin 1

int freq_to_mod(float freq, float pre) {
    float mod;
    mod = (48000000) / pre * (1 / freq);
    mod = (int)mod;
    return mod;
}


typedef enum notes
{
	C = 262, D = 294, E = 330, F = 329, G = 392, A = 440, B = 494
} notes;

void playNotes(notes note)
{
		int mod = freq_to_mod(note, 128);
		TPM1_C0V = mod;
		TPM1->MOD = mod/2;
}

int mod_val = 7500;
/*intiPWM() */
void initPWM(void)
{
	//enable clock
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	//clear mux, set mux mode to alt. 3 (TPMCh0)
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
	
	//clear mux, set mux mode to alt. 3 (TPMCh1)
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
	
	//enable clock for timer
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	//clear clock source & set clock source to MCGFLLCLK clock
	SIM->SOPT2 &= ~ SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
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
	
	TPM1_C0V = mod_val/2;
	while(1)
	{
		playNotes(A);
	}
}