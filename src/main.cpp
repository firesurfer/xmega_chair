
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "uart.h"
#include "adc.h"
#include "powermanager.h"
#include "uart_parser.h"
#include "speedcontroller.h"
#include "interrupts.h"

#define WDT_Reset()	asm("wdr") //( watchdog_reset( ) )

PowerManager pmanager;
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

bool locked = true;
void handle_command(void* obj,uint8_t cmd, uint16_t& data)
{
    switch (cmd) {
    case 0:
        scontroller.lock();
        pmanager.lock();
        locked = true;
        break;
    case 1:
        scontroller.lock();
        break;
    case 2:
        pmanager.unlock();
        locked = false;
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
    case 7:
        scontroller.pid_controller.kP = data;
        break;
    case 8:
        scontroller.pid_controller.kI = data;
        scontroller.pid_controller.sum_max = (scontroller.pid_controller.maximum*64) / scontroller.pid_controller.kI;//uint32_t needed, if maximum*64 > 16bit
        break;
    case 9:
        scontroller.pid_controller.kD = data;
        break;
    case 10:
        scontroller.pid_controller.maximum = data;
        scontroller.pid_controller.sum_max = (scontroller.pid_controller.maximum*64) / scontroller.pid_controller.kI;
    case 11:
        scontroller.set_weak(WheelPosition::FrontLeft,data); //Field weakening
        break;
    case 12:
        scontroller.set_weak(WheelPosition::FrontRight,data);
        break;
    case 13:
        scontroller.set_weak(WheelPosition::RearLeft,data);
        break;
    case 14:
        scontroller.set_weak(WheelPosition::RearRight,data);
        break;
    case 15:
        if(data > 2)
        {
            break;
        }
        scontroller.set_mode((DriveMode)data);
        if((DriveMode)data == DriveMode::CombinedDrive)
            led1.set();
        else {
            led1.clear();
        }
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
    TCC0.PER = 500;
    TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;
}

void setup_watchdog()
{
    CCP = CCP_IOREG_gc;
    WDT.CTRL = WDT_PER_512CLK_gc | WDT_ENABLE_bm | WDT_CEN_bm ;
    while(  WDT.STATUS & WDT_SYNCBUSY_bm )
    {

    }
}

int main(void)
{
    setup_clock();
    cmdparser.set_command_handler(handle_command, nullptr);
    PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
    // setup_watchdog();
    WDT_Reset();
    setup_counter();
    pmanager.lock();
    scontroller.lock();

    sei();

    uartc0.transmit_it("Hello world");
    while(1)
    {

        _delay_ms(100);
        pmanager.task_switches();
        if(!locked)
            scontroller.send_speed_to_pc();
        WDT_Reset();
    }
}
