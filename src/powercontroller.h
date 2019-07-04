#pragma once
#include <avr/io.h>

class PowerController
{
public:
    PowerController();
    void task_switches();
    void power_off();
    void power_on();
private:

    uint8_t task_count = 0;

    PORT_t& m_power_port = PORTC;
    uint8_t m_power_left = PIN5;
    uint8_t m_power_right = PIN4;

    PORT_t& m_switch_port = PORTR;
    uint8_t m_switch_left = PIN0;
    uint8_t m_switch_right = PIN1;

};
