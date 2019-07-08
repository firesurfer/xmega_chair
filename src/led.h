
#pragma once

#include <avr/io.h>

class Led
{
public:
    Led(PORT_t& port, uint8_t pin);
    void set();
    void clear();
    void toggle();

private:
    PORT_t& m_port;
    uint8_t m_pin;
};
