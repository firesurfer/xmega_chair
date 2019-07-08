
#pragma once

#include <avr/io.h>

class Led
{
public:
    Led(PORT_t& port, uint8_t pin);
    void set(){m_port.OUTSET = (1<<m_pin);}
    void clear(){m_port.OUTCLR = (1<<m_pin);}
    void toggle(){m_port.OUTTGL = (1<<m_pin);}

private:
    PORT_t& m_port;
    uint8_t m_pin;
};
