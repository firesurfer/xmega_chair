#include "led.h"

Led::Led(PORT_t &port, uint8_t pin):
    m_port(port),
    m_pin(pin)
{
    port.DIR |= (1<<pin);
    port.OUTCLR = (1<<pin);
}

