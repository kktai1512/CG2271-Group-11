#include "MKL25Z4.h"
#include "myDataPacket.h"
#define UART_RX_PORTE23 23
#define BAUD_RATE 9600
#define UART2_INT_PRIO 128


void initUART2(uint32_t baud_rate);
void UART2_Transmit_Poll(uint8_t data) ;
uint8_t UART2_Receive_Poll(void) ;
static void delay(volatile uint32_t nof);
//void UART2_IRQHandler() ;

