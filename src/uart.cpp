#include "uart.h"
#include <avr/interrupt.h>

Uart::Uart(USART_t &usart, PORT_t & port, uint8_t pin, int baudrate):
    m_usart(usart)
{
    port.DIRSET = (1<<pin);

    m_usart.BAUDCTRLB = ( unsigned char ) ( baudrate>>8 );
    m_usart.BAUDCTRLA = ( unsigned char ) baudrate;

    m_usart.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
    m_usart.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc;
    m_usart.CTRLA = USART_DREINTLVL_MED_gc | USART_RXCINTLVL_HI_gc;
}

void Uart::transmit_it(char c)
{
    while(tx_buf.full());
    tx_buf.push_back(c);
}

void Uart::transmit_it(const char *s)
{
    while(*s)
    {
        transmit_it(*s);
        ++s;
    }
}

void Uart::transmit_buffer_it(const uint8_t *buf, uint8_t len)
{
    for(uint8_t i=0;i<len;++i)
        transmit_it(buf[i]);
}

void Uart::transmit(char *str)
{
    while(*str)
    {
        while(!(m_usart.STATUS & USART_DREIF_bm));
        m_usart.DATA = *str;
        str++;
    }
}

void Uart::transmit_buffer(uint8_t *buffer, uint8_t length)
{
    for (uint8_t i = 0; i < length;i++) {
        while(!(m_usart.STATUS & USART_DREIF_bm));
        m_usart.DATA = buffer[i];
    }
}

uint8_t Uart::read()
{
    while(!(m_usart.STATUS & USART_RXCIF_bm));
    return m_usart.DATA;
}

void Uart::tx_complete_interrupt()
{

}

void Uart::dre_interrupt()
{
    register uint8_t data;
    if(tx_buf.pop_front(data)){
        m_usart.DATA = data;
    }else{
        m_usart.CTRLA &= ~USART_DREINTLVL_gm;
    }
}



