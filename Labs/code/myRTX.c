#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
  for (;;) {
      ledControl(RED_LED, led_on);
      osDelay(1000); 
      /*Round-Robin Thread switching
       -> go to RTX_config.h -> change OS_ROBIN_ENABLE to 0
       then use normal delay function
       */
      ledControl(RED_LED, led_off);
      osDelay(1000);
  }
}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
  osKernelStart();                      // Start thread execution
  for (;;) {}
}

-
/*----------------------------------------------------------------------------
 * Led colors
 *---------------------------------------------------------------------------*/


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

typedef enum led_colors {
  red_led = RED_LED,
  green_led = GREEN_LED,
	blue_led = BLUE_LED,
} led_colors_t;

char led_mapping[3] [2] = {{0, red_led}, {1, green_led}, {2, blue_led}};

void ledcontrol (led_colors_t colour, string onOff)
{
    if (onOff = "offRGB") {
        return;
    }
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