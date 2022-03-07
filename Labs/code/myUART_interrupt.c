#include system_MKL25Z4.h
void Init_UART2(uint32_t baud_rate) 
{
    NVIC_SetPriority(UART2_IRQn, 128); 
    NVIC_ClearPendingIRQ(UART2_IRQn); 
    NVIC_EnableIRQ(UART2_IRQn);
    UART2->C2 | = UART_C2_TIE_MASK | UART_C2_RIE_MASK;
    UART2->C2 | = UART_C2_RIE_MASK; 
    Q_Init(&TxQ);
    Q_Init(&RxQ);
}


void UART2_IRQHandler(void) 
{ 
    NVIC_ClearPendingIRQ(UART2_IRQn); 
    if (UART2->S1 & UART_S1_TDRE_MASK) {
        // can send another character if (!Q_Empty(&TxQ)) {
        UART2->D = Q_Dequeue(&TxQ); } 
    else {
        // queue is empty so disable tx 
        UART2->C2 &= ~UART_C2_TIE_MASK;
    } 
}

void UART2_IRQHandler(void) {
    if (UART2->S1 & UART_S1_RDRF_MASK) { 
        // received a character
        if (!Q_Full(&RxQ)) {
            Q_Enqueue(&RxQ, UART2->D);
        } 
        else {
            // error - queue full.
            while (1)
        }
    }
 }

 void UART2_IRQHandler(void) 
 {
    if (UART2->S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK 
        | UART_S1_FE_MASK |UART_S1_PF_MASK)) 
    {
    // handle the error
    // clear the flag
    }
 }