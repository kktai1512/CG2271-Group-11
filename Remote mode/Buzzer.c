#define SW_POS			6
#define MASK(x) 		(1<< (x))
#include "MKL25Z4.h"                    // Device header
#define PTC8_Pin 8
#define PTC9_Pin 9
#define arrayLength(array) (sizeof((array))/sizeof((array)[0]))
int volatile stop_song_f = 0;
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
	
	PORTB->PCR[0] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[0] |= PORT_PCR_MUX(3);	
	
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


static void delay(volatile uint32_t nof)
{
	while(nof!=0)
	{
		__asm("NOP");
		nof--;
	}
}

void play_song(int size, int score[],int note[]){
	
	for (int i=0;i<size;i++){
		
			if (score[i]<7){
			TPM0_C4V = freq_to_mod(note[score[i]],256)/64;
			TPM0->MOD = freq_to_mod(note[score[i]],256);
			osDelay(250);
			}
			else if (score[i]==7){
		  TPM1_C0V = 0;
			TPM1->MOD = 0;
			osDelay(250);
			}
	  }
	}


	
void play_note(int note_number, int size,int score[], int note[]) {
	int s_note = score[note_number%size];
		if (s_note<7){
			TPM1_C0V = freq_to_mod(note[s_note],256)/64;
			TPM1->MOD = freq_to_mod(note[s_note],256);
			osDelay(250);
			}
			else if (s_note==7){
		  TPM1_C0V = 0;
			TPM1->MOD = 0;
			osDelay(250);
			}
}


	void stop_song() {
		stop_song_f = 1;
	}
