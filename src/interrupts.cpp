#include "interrupts.h"

extern Uart uartc0;
extern Uart uartc1;
extern Uart uartd0;

ISR(USARTC0_TXC_vect)
{
    uartc0.tx_complete_interrupt();
}

ISR(USARTC0_RXC_vect)
{
    uartc0.rx_complete_interrupt();
}


ISR(USARTC1_TXC_vect)
{
    uartc1.tx_complete_interrupt();
}

ISR(USARTC1_RXC_vect)
{
    uartc1.rx_complete_interrupt();
}

ISR(USARTD0_TXC_vect)
{
    uartd0.tx_complete_interrupt();
}

ISR(USARTD0_RXC_vect)
{
    uartd0.rx_complete_interrupt();
}

