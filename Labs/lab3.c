#include "MKL25Z4.h"                    // Device header

#define RED_LED 18   // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1   // PortD Pin 1
#define SWITCH 6     // PortD Pin 6
#define MASK(x) (1 << (x))

typedef enum {red, green, blue} LED;
LED active = red;
volatile unsigned int toggle = 1;

void toggleLED(LED led) {
    static const unsigned int masks[] = {MASK(RED_LED), MASK(GREEN_LED), MASK(BLUE_LED)};
    static const uint32_t ports[] = {PTB, PTB, PTD};
    GPIO_Type *port = (GPIO_Type *) ports[led];
    port->PTOR |= masks[led];
}

void updateLED(void) {  
    switch (active) {
        case red:
            toggleLED(red);
            toggleLED(active = green);
            break;
        case green:
            toggleLED(green);
            toggleLED(active = blue);
            break;
        case blue:
            toggleLED(blue);
            toggleLED(active = red);
            break;
    }
}

void SysTick_Handler(void) {
    if (toggle) {
        updateLED();
    }
}

void PORTD_IRQHandler() {
    // Clear Pending IRQ
    NVIC_ClearPendingIRQ(PORTD_IRQn);
    // Updating some variable / flag
    if ((PORTD->ISFR & MASK(SWITCH))) {
        toggle = !toggle;
    }
    //Clear INT Flag
    PORTD->ISFR = 0xffffffff;
}

void InitGPIO(void) { 
    // Enable Clock to PORTB and PORTD 
    SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK)); 
     
    // Configure MUX settings to make all 3 pins GPIO    
    PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK; 
    PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1); 
     
    PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK; 
    PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1); 
     
    PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK; 
    PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);

    PORTD->PCR[SWITCH] |= (PORT_PCR_MUX(1) | PORT_PCR_PS_MASK
                         | PORT_PCR_PE_MASK | PORT_PCR_IRQC(0xb));
     
    // Set Data Direction Registers for PortB and PortD 
    PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED)); 
    PTD->PDDR |= MASK(BLUE_LED);
    PTD->PDDR &= ~MASK(SWITCH);
    
    // On red and off the rest
    PTD->PSOR |= MASK(BLUE_LED);
    PTB->PSOR |= MASK(GREEN_LED);
    PTB->PCOR |= MASK(RED_LED); 
}

int main(void) {
    InitGPIO();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 2);
    NVIC_DisableIRQ(PORTD_IRQn);
    NVIC_ClearPendingIRQ(PORTD_IRQn);
    NVIC_EnableIRQ(PORTD_IRQn);
    while(1);
}
