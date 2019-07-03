#include "led.h"

Led::Led(PORT_t &port, uint8_t pin):
    m_port(port),
    m_pin(pin)
{
    port.DIR |= (1<<pin);
    port.OUTCLR = (1<<pin);
}

void Led::set()
{
    m_port.OUTSET = (1<<m_pin);

}

void Led::clear()
{
    m_port.OUTCLR = ( 1<<m_pin);
}

void Led::toggle()
{
    m_port.OUTTGL = (1<<m_pin);
}
