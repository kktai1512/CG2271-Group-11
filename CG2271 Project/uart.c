#include "uart.h"

volatile uint8_t rx_interruptData =0;

void initUART2(uint32_t baud_rate) {
	
	uint32_t divisor, bus_clock;
	
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	
	//mux is at 4 because thats the configuration for uart
	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	//clearing the bits for the transmitter and receiver 
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	
	bus_clock = DEFAULT_SYSTEM_CLOCK/2;
	divisor = bus_clock / (baud_rate *16);
	UART2->BDH = UART_BDH_SBR(divisor>>8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0 ;
	UART2->S2 = 0 ;
	UART2->C3 = 0 ;
	
	//enabling the transmitter and receiver
	UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	
}

void UART2_Transmit_Poll(uint8_t data) {
	while(!(UART2->S1 & UART_S1_TDRE_MASK));
	UART2->D = data;
}

uint8_t UART2_Receive_Poll(void) {
	while(!(UART2->S1 & UART_S1_RDRF_MASK));
	return (UART2->D);
}



void UART2_IRQHandler() {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_interruptData = UART2->D;
	}

}