//#include "MKL25Z4.h"
#define MASK(x) (1 << (x))
#include "led.h"

int led[] = {11, 10, 6, 5, 4, 3, 0, 7, RED}; //PortC
volatile int curr = 0, prev = NUM_GREEN - 1;
volatile int running = 0, toggle = 0;

void shiftGreenLED(void) {   
    PTC->PCOR |= (MASK(led[prev]));
    PTC->PSOR |= (MASK(led[curr]));
    prev = curr;
    curr = (curr == NUM_GREEN - 1) ? 0 : curr + 1;
}

void setGreenLED(int set) {
	if(set) {
		PTC->PSOR |= (MASK(11)| MASK(10)| MASK(6)| MASK(5)| MASK(4) | MASK(3)| MASK(0)| MASK(7));
	} else {
		PTC->PSOR &= !(MASK(11)| MASK(10)| MASK(6)| MASK(5)| MASK(4) | MASK(3)| MASK(0)| MASK(7));
	}
}


void toggleLED(void) {
    PTC->PTOR |= (MASK(RED));
}

void toggleRedLED() {
		PTC->PTOR |= (MASK(RED));
}

void offRedLED() {
	PTC->PTOR &= ~(MASK(RED));
}
/*
void SysTick_Handler(void) {
    if (running) {
        shiftLED();
        toggle = !toggle;                           // 1 cycle delay for 
        if (toggle) {
            toggleLED();
        }        
    }
    else {
        toggleLED();
        for (int j = 0; j < NUM_GREEN; j++) {
            PTC->PSOR |= (MASK(led[j]));            // On all LEDs
        }
    }
}
*/

void InitLED(void) {
    SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);           // Enable Clock to PORTC 
    for (int j = 0; j < NUM_LED; j++) {
        PORTC->PCR[led[j]] &= ~PORT_PCR_MUX_MASK;   // Configure MUX settings to make all pins GPIO
        PORTC->PCR[led[j]] |= PORT_PCR_MUX(1);
        PTC->PDDR |= (MASK(led[j]));                // Set Data Direction Registers for PortB and PortD
        PTC->PCOR |= (MASK(led[j]));                // Off all LEDs
    }
}

/*
int main(void) {
    InitLED();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 4);            //250 ms interrupts
    while(1);
}
*/