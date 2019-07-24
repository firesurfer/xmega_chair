
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "uart.h"
#include "adc.h"
#include "powermanager.h"
#include "uart_parser.h"
#include "speedcontroller.h"
#include "interrupts.h"



PowerManager pcontroller;
Uart uartc0(USARTC0, PORTC,PIN3,UART_BAUD_SELECT_XMEGA(115200, F_CPU));
Uart uartc1(USARTC1, PORTC,PIN7,UART_BAUD_SELECT_XMEGA(115200, F_CPU));
Uart uartd0(USARTD0, PORTD,PIN3,UART_BAUD_SELECT_XMEGA(115200, F_CPU));

ADC steeringAdc(ADCA);
SpeedController scontroller(uartc1, uartd0, steeringAdc);
Led led1(PORTE,0);
Led led2(PORTE,1);
Led led3(PORTE,2);
Led led4(PORTE,3);
UartParser cmdparser(uartc0);






void handle_command(uint8_t cmd, uint16_t& data)
{
    switch (cmd) {
    case 0:
        scontroller.lock();
        pcontroller.lock();
        break;
    case 1:
        scontroller.lock();
        break;
    case 2:
        pcontroller.unlock();
        break;
    case 3:
        scontroller.unlock();
        break;
    case 4:
        scontroller.set_speed((int16_t)data);
        break;
    case 5:
        scontroller.set_angle((int16_t)data);
        break;

    case 6:
        led1.toggle();
        break;

    }
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

void setup_counter()
{
    TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
    TCC0.PER = 5000;
    TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;

}

int main(void)
{
    setup_clock();
    cmdparser.set_command_handler(handle_command);
    PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
    setup_counter();
    pcontroller.unlock();
    pcontroller.off();

    scontroller.unlock();

    sei();

    uartc0.transmit_it("Hello world");
    while(1)
    {
        led1.toggle();
        _delay_ms(200);
        pcontroller.task_switches();
    }
}
