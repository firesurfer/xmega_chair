#pragma once
#include <avr/io.h>

class PowerController
{
public:
    PowerController();
    void task_switches();
    void off();
    void on();
    /**
     * @brief lock - power off and disable switches and on function
     */
    void lock();
    /**
     * @brief unlock - allow switches and on function
     */
    void unlock();
private:

    bool m_power_on = false;

    PORT_t& m_power_port = PORTC;
    uint8_t m_power_left = PIN5;
    uint8_t m_power_right = PIN4;

    PORT_t& m_switch_port = PORTR;
    uint8_t m_switch_left = PIN0;
    uint8_t m_switch_right = PIN1;

};
