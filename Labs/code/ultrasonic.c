#define ECHO_IN 1 //PORTA_1 Timer 2 C0
#define TRIG_OUT 4 //PORTD_4 GPIO

#define SPEED_OF_SOUND 343000.0


#define mod_val 18000;
#define PS 128;

static volatile bool started = false;



volatile float distance = 0.0;

void initTimer(void) {
    //Enable clock gating for timer 2
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	//Clear clock source & set clock source to MCGFLLCLK clock
	SIM->SOPT2 &= ~ SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	//todo
	TPM2->MOD = mod_val;

	//clock mode and prescaler
	TPM2->SC &= ~((TPM_SC_CMOD_MASK)|(TPM_SC_PS_MASK));
	//LPTPM increments on every counter clock, prescaler = 128
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    //set center-aligned pwm to 0 (upcounting mode)
    TPM2->SC &= ~(TPM_SC_CPWMS_MASK)


	//edge_or_level_select, channel mode select
	TPM2_C0SC &= ~ ((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) |TPM_CnSC_MSA_MASK) ;
	//capture on both rising and falling edge
    TPM2_C0SC |= (TPM_CnSC_ELSA(1) | TPM_CnSC_ELSA(1));
    
    //for input capture at both edges, cpwms 0, msbmsa 00, elsb elsa 11.
   
    //interrupts
    TPM2_C0SC |= TPM_CnSC_CHIE(1);
    TPM2_C0SC |= TPM_CnSC_TOIE(1);

    NVIC_EnableIRQ(TPM2_IRQn);
    __enable_irq();
}

/* Delay routine */
static void delay (volatile uint32_t nof) {
    while (nof!=0) {
    __asm ("NOP");
    nof--;
    }
}

static void initGPIO(void) {

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

void TPM2_IRQHandler(void) {
    NVIC_ClearPendingIRQ(TPM2_IRQn);

    //channel flag
    if (TPM2_C0SC & TPM_CnSC_CHF_MASK) {
        //clear flag first 
        TPM2_C0SC |= TPM_CnSC_CHF(1);
        
        //rising edge
        if (!started) {
            started = true;
            //starts from 0;
            distance = 0;
            resetCount(); 
        }
        //falling edge
        else {
            started = false;
            distance += calculateDistance(TPM2->C0V);
            resetCount();
        }
    }

    //if overflow
    if (TPM1->SC & TPM_SC_TOF_MASK) {
        TPM1->SC |= TPM_SC_TOF(1); //clear flag
        if (started) {
            distance = calculateDistance(mod_val);
            resetCount();
        }

    }

}

void sendPulse(void) {
    PTD->PSOR |= MASK(TRIG_OUT);
    //48 means 10 micro;
    delay(48);
    PTD->PCOR |= MASK(TRIG_OUT);
}



void resetCount(void) {
    TPM2->CNT =0;
}

float calculateDistance(int steps) {
    float time = steps*(PS)/48000000;
    return SPEED_OF_SOUND*time;
}

float getDistance() {
    return distance;
}

int main void() {
    initGPIO();
    initTimer();
    while(1) {
        sendPulse();
    }
}