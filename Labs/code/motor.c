#include "MKL25Z4.h"
#define PTB0_Pin 0
#define SW_POS 6 //PortD Pin 6
#define PTB1_Pin 1
#define MASK(x) (1 << (x))


int mod_val = 7500;
volatile int spin = 0;

void motorDutyCycle(int percent) {
	TPM1_C0V = mod_val * percent / 100;
}

void forward (int percent) {
	TPM1_C0V = mod_val * percent/100;
	TPM1_C1V = 0;
}

void backward (int percent) {
	TPM1_C1V = mod_val * percent/100;
	TPM1_C0V = 0;
	
}

void initSwitch(void)
{
    // enable clock for PortD
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

    //* Select GPIO and enable pull-up resistors and interrupts on falling edges of pin connected to switch*/
    PORTD->PCR[SW_POS] |= (PORT_PCR_MUX(1)| PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0x0a));
    //PE is pull enable, PS then select it to be pullup

    // Set PORT D Switch bit to input
    PTD->PDDR &= ~MASK(SW_POS);

    //Enable Interrupts
    NVIC_SetPriority (PORTD_IRQn, 2);
    NVIC_ClearPendingIRQ (PORTD_IRQn);
    NVIC_EnableIRQ (PORTD_IRQn);
}


void PORTD_IRQHandler()
{
    // Clear Pending IRQ
    NVIC_ClearPendingIRQ(PORTD_IRQn);
    // Updating some variable / flag
    if (spin == 0) {
			spin = 1;
		}
		else {
			spin = 0;
		}
    //Clear INT Flag
    PORTD->ISFR |= MASK(SW_POS);
 }
 

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


int main(void)
{
	SystemCoreClockUpdate();
	initSwitch();
	initPWM();
	
	//TPM1_C0V = mod_val/2;
	while(1)
		
	{
		if (spin == 1) {
			forward(50);
		}
		else {
			backward(50);
		}
		
	}
}