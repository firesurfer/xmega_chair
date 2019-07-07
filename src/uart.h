#pragma once

#include <avr/io.h>

#define _BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,bscale) (                \
((bscale) < 0) ?                                                      \
  (int)((((float)(f_cpu)/(16*(float)(baud)))-1)*(1<<-(bscale))+.5f)        \
: (int)((float)(f_cpu)/((1<<(bscale))*16*(float)(baud))-.5f) )

#define _BSCALE(f_cpu,baud) (                                         \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-7) < 4096) ? -7 :              \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-6) < 4096) ? -6 :              \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-5) < 4096) ? -5 :              \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-4) < 4096) ? -4 :              \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-3) < 4096) ? -3 :              \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-2) < 4096) ? -2 :              \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-1) < 4096) ? -1 :              \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,0) < 4096) ? 0 :                \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,1) < 4096) ? 1 :                \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,2) < 4096) ? 2 :                \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,3) < 4096) ? 3 :                \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,4) < 4096) ? 4 :                \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,5) < 4096) ? 5 :                \
(_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,6) < 4096) ? 6 :                \
7 )

#define BSEL(f_cpu,baud)                                              \
  _BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,_BSCALE(f_cpu,baud))

#define UART_BAUD_SELECT_XMEGA(baudRate,xtalCpu) ((BSEL(xtalCpu,baudRate)&0xFFF)|((_BSCALE(xtalCpu,baudRate)&0xF)<<12))


class Uart
{
public:
    Uart(USART_t& usart, PORT_t &port, uint8_t pin, int baudrate);
    void transmit(char* str);
    void transmit_buffer(uint8_t* buffer, uint8_t length);
    void transmit_it();
    void transmit_dma();
    uint8_t read();

    void tx_complete_interrupt();
    void dre_interrupt();
    void rx_complete_interrupt();
private:
    USART_t& m_usart;
    int m_baudrate;

    const uint8_t transmit_buffer_size = 10;

};
