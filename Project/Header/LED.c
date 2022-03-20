#include "LED.h";

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

void offRGB (void)
{
 PTB->PSOR = (MASK (RED_LED) | MASK (GREEN_LED));
 PTD->PSOR = MASK (BLUE_LED) ;
}


void ledControl (led_colors_t colour, onOff_t onOff)
{
    offRGB () ;
    switch (colour)
    {
    case RED_LED:
        PTB->PCOR = MASK (RED_LED) & (onOff << (RED_LED-1) );
        break;
    case GREEN_LED:
        PTB->PCOR = MASK (GREEN_LED) & (onOff << (GREEN_LED-1));
        break;
    case BLUE_LED:
        PTD->PCOR = MASK (BLUE_LED) & (onOff << (BLUE_LED-1));
        break;
    default:
        offRGB ();
    }
}