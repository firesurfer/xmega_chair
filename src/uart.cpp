#include "uart.h"

Uart::Uart(USART_t &usart, PORT_t & port, uint8_t pin, int baudrate):
    m_usart(usart),
    m_baudrate(baudrate)
{
    m_usart.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
    m_usart.CTRLC = USART_CMODE_enum::USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_enum::USART_CHSIZE_8BIT_gc;


    m_usart.BAUDCTRLB = ( unsigned char ) ( baudrate>>8 );
    m_usart.BAUDCTRLA = ( unsigned char ) baudrate;

    port.DIRSET = (1<<pin);


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

uint8_t Uart::read()
{
    while( !(m_usart.STATUS & USART_RXCIF_bm) ); //Interesting DRIF didn't work.
        return m_usart.DATA;
}

void Uart::tx_complete_interrupt()
{

}

void Uart::dre_interrupt()
{

}

void Uart::rx_complete_interrupt()
{

}


