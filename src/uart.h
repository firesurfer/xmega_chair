#pragma once

#include <avr/io.h>
#include "ringbuf.h"

#define _BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,bscale) (                  \
    ((bscale) < 0) ?                                                    \
      (int)((((float)(f_cpu)/(16*(float)(baud)))-1)*(1<<-(bscale))+.5f) \
    : (int)((float)(f_cpu)/((1<<(bscale))*16*(float)(baud))-.5f) )

#define _BSCALE(f_cpu,baud) (                                           \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-7) < 4096) ? -7 :            \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-6) < 4096) ? -6 :            \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-5) < 4096) ? -5 :            \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-4) < 4096) ? -4 :            \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-3) < 4096) ? -3 :            \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-2) < 4096) ? -2 :            \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,-1) < 4096) ? -1 :            \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,0) < 4096) ? 0 :              \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,1) < 4096) ? 1 :              \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,2) < 4096) ? 2 :              \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,3) < 4096) ? 3 :              \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,4) < 4096) ? 4 :              \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,5) < 4096) ? 5 :              \
    (_BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,6) < 4096) ? 6 :              \
    7 )

#define BSEL(f_cpu,baud)                                                \
    _BAUD_BSEL_FROM_BAUDSCALE(f_cpu,baud,_BSCALE(f_cpu,baud))

#define UART_BAUD_SELECT_XMEGA(baudRate,xtalCpu) ((BSEL(xtalCpu,baudRate)&0xFFF)|((_BSCALE(xtalCpu,baudRate)&0xF)<<12))


class Uart
{
public:
    typedef void (*rx_handler_t)(void*,uint8_t);
    Uart(USART_t& usart, PORT_t &port, uint8_t pin, int baudrate);
    void transmit_it(char c);
    void transmit_it(const char *s);
    void transmit_it(const uint8_t *buf, uint8_t len);

    void transmit(char s);
    void transmit(char* str);
    void transmit(uint8_t* buffer, uint8_t length);

    uint8_t read();

    void dre_interrupt(){
        register uint8_t data;
        if(tx_buf.pop_front(data)){
            m_usart.DATA = data;
        }else{
            m_usart.CTRLA &= ~USART_DREINTLVL_gm;
        }
    }
    void rx_complete_interrupt(){
        if(rx_handler  && rx_handler_obj)
            rx_handler(rx_handler_obj, m_usart.DATA);
    }
    void set_rx_handler(rx_handler_t handler, void *handler_obj){
        rx_handler = handler;
        rx_handler_obj = handler_obj;
    }

private:
    USART_t& m_usart;
    rx_handler_t rx_handler = nullptr;
    void *rx_handler_obj = nullptr;

    ringbuf_t<10> tx_buf;

};


