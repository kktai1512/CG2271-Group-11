#include "MKL25Z4.h"

#define SW_POS 6 //PortD Pin 6
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1

#define MASK(x) (1 << (x))

unsigned int counter = 0;

typedef enum led_colors {
  red_led = RED_LED,
  green_led = GREEN_LED,
	blue_led = BLUE_LED,
} led_colors_t;

char led_mapping[3] [2] = {{0, red_led}, {1, green_led}, {2, blue_led}};


void initLED(void)
{
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
}


void PORTD_IRQHandler()
{
    // Clear Pending IRQ
    NVIC_ClearPendingIRQ(PORTD_IRQn);
    // Updating some variable / flag
    if (counter < 2) {
        counter++;
    }
    else {
        counter = 0;
    }
    //Clear INT Flag
    PORTD->ISFR |= MASK(SW_POS);
 }

void offRGB (void)
{
 PTB->PSOR = (MASK (RED_LED) | MASK (GREEN_LED));
 PTD->PSOR = MASK (BLUE_LED) ;
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

void ledcontrol (led_colors_t colour)
{
    offRGB () ;
    switch (colour)
    {
    case RED_LED:
        PTB->PCOR = MASK (RED_LED);
        break;
    case GREEN_LED:
        PTB->PCOR = MASK (GREEN_LED) ;
        break;
    case BLUE_LED:
        PTD->PCOR = MASK (BLUE_LED) ;
        break;
    default:
        offRGB ();
    }
}

static void delay (volatile uint32_t nof) {
    while (nof!=0) {
    __asm ("NOP");
    nof--;
    }
}

int main(void)
{
    initSwitch();
    initLED();
    offRGB();
    while(1)
    {
        ledcontrol(led_mapping[counter][1]);
    } 
}
