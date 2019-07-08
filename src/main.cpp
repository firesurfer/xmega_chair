
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "uart.h"
#include "adc.h"
#include "powercontroller.h"
#include "uart_parser.h"

PowerController pcontroller;
Uart uartc0(USARTC0, PORTC,PIN3,UART_BAUD_SELECT_XMEGA(115200, F_CPU));
Uart uartc1(USARTC1, PORTC,PIN7,UART_BAUD_SELECT_XMEGA(115200, F_CPU));
Uart uartd0(USARTD0, PORTD,PIN2,UART_BAUD_SELECT_XMEGA(115200, F_CPU));
ADC steeringAdc(ADCA);
Led led1(PORTE,0);
Led led2(PORTE,1);
Led led3(PORTE,2);
Led led4(PORTE,3);
UartParser cmdparser(uartc0);

void handle_command(uint8_t cmd, uint16_t& data)
{
    //TODO
}

void setup_clock()
{
    CCP = CCP_IOREG_gc;
    OSC.CTRL |= OSC_RC32MEN_bm; //32MHZ internal oscillator
    while(!(OSC.STATUS & OSC_RC32MRDY_bm));
    //Factor 4 PLL
    OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc | 0x04;
    //Enable PLL
    OSC.CTRL |= OSC_PLLEN_bm;
    while(!(OSC.STATUS & OSC_PLLRDY_bm));
    CCP = CCP_IOREG_gc;
    //Use PLL as system clock
    CLK.CTRL = CLK_SCLKSEL_PLL_gc;
}

int main(void)
{
    setup_clock();

    led1.set();
    led2.set();
    led3.set();
    led4.set();

    pcontroller.unlock();
    pcontroller.on();

    while(1)
    {
        led1.toggle();
        _delay_ms(500);
    }

}
