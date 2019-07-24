
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "uart.h"
#include "adc.h"
#include "powermanager.h"
#include "uart_parser.h"
#include "speedcontroller.h"


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



    }
}

ISR(TCC2_LCMPB_vect)
{


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

}

int main(void)
{
    setup_clock();
    PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;

    pcontroller.unlock();

    steeringAdc.start_it();
    sei();



    while(1)
    {
        led1.toggle();
        _delay_ms(100);
        pcontroller.task_switches();
        char buffer[10];
        itoa(steeringAdc.lastResult(0), buffer,10);
        uartc0.transmit_it(buffer);
        uartc0.transmit_it('\n');
        /*scontroller.send_packet(1,200,uartc1);
        scontroller.send_packet(2,-200,uartc1);
        scontroller.send_packet(1,200,uartd0);
        scontroller.send_packet(2,-200,uartd0);*/



    }
}
