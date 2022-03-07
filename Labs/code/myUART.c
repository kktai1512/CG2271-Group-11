#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128


/* Init UART2 */

void initUART2 (uint32_t baud_rate) 
{
    uint32_t divisor, bus_clock;
    SIM -> SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM -> SCGC5 |= SIM_SCGC4_PORTE_MASK;

    PORTE->PCR[UART_TX_PORTE22] &= ~ PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_TX_PORTE22] |= ~ PORT_PCR_MUX_(4);

    PORTE->PCR[UART_TX_PORTE23] &= ~ PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_TX_PORTE23] |= ~ PORT_PCR_MUX_(4);

    UART->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));

    bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
    divisor = bus_clock / (baud_rate * 16);
    UART->BDH = UART_BDH_SBR(divisor >> 8);
    UART2->BDL = UART_BDL_SBR(divisor);

    UART2->C1 = 0;
    UART2->S2 = 0;
    UART2->C3 = 0;

    UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
}

/* UART2 Transmit Poll */
void UART2_Transmit_Poll (uint8_t data)
{
    while(!(UART2->S1) & UART_S1_TDRE_MASK));
    UART2->D = data;
}


/* UART2 Receive Poll */
uint8_t UART2_Receive_Poll (void)
{
    while(!(UART2->S1) & UART_S1_RDRF_MASK));
    return UART2->D;
}

/* Delay routine */
static void delay (volatile uint32_t nof) {
    while (nof!=0) {
    __asm ("NOP");
    nof--;
    }
}

/* Main */
int main(void)
{
    uint8_t rx_data = 0x69;

    SystemCoreClockUpdate();
    initUART2(BAUD_RATE);

    while(1)
    {
        UART2_Transmit_Poll(0x69);
        delay(0x80000);
    }
}