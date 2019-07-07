#pragma once

#include <avr/io.h>
#include "ringbuf.h"

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
    typedef void (*rx_handler_t)(uint8_t);
    Uart(USART_t& usart, PORT_t &port, uint8_t pin, int baudrate);
    void putc(char c){while(tx_buf.full());tx_buf.push_back(c);}
    void puts(const char *s){while(*s){putc(*s);++s;}}
    void write(const uint8_t *buf, uint8_t len){for(uint8_t i=0;i<len;++i)putc(buf[i]);}
//    void transmit(char* str);
//    void transmit_buffer(uint8_t* buffer, uint8_t length);
//    void transmit_it();
//    void transmit_dma();
//    uint8_t read();

//    void tx_complete_interrupt();
//    void dre_interrupt();
    inline void rx_complete_interrupt(){if(rx_handler)rx_handler(m_usart.DATA);}//Maybe get byte via param from ISR instead of m_usart indirect read
    void set_rx_handler(rx_handler_t handler);
private:
    USART_t& m_usart;
    rx_handler_t rx_handler;

    const uint8_t transmit_buffer_size = 10;

public://More efficient, if ISR can write directly to ringbuf, instead of function call
    ringbuf_t<10> tx_buf;
    //ringbuf_t<transmit_buffer_size> rx_buf;
};

extern Uart uartc0;
extern Uart uartc1;
extern Uart uartd0;
