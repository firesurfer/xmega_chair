#include "interrupts.h"
#include "speedcontroller.h"
#include "led.h"
extern Uart uartc0;
extern Uart uartc1;
extern Uart uartd0;
extern ADC steeringAdc;
extern Led led1;
extern SpeedController scontroller;

ISR(USARTC0_RXC_vect)
{
    uartc0.rx_complete_interrupt();
}
ISR(USARTC0_DRE_vect)
{
    uartc0.dre_interrupt();
}

ISR(USARTC1_RXC_vect)
{
    uartc1.rx_complete_interrupt();
}
ISR(USARTC1_DRE_vect)
{
    uartc1.dre_interrupt();
}

ISR(USARTD0_RXC_vect)
{
    uartd0.rx_complete_interrupt();
}
ISR(USARTD0_DRE_vect)
{
    uartd0.dre_interrupt();
}

ISR(ADCA_CH3_vect)
{
    steeringAdc.ch3_interrupt();
}

ISR(TCC0_OVF_vect)
{
    scontroller.update();
    steeringAdc.start_it();
}
