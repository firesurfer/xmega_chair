#include "powercontroller.h"

PowerController::PowerController()
{
    PORTC.DIRSET = (1<<m_power_left) | ( 1<< m_power_right);
    PORTC.OUTCLR = (1<<m_power_left) | ( 1<< m_power_right);
    m_switch_port.DIRCLR = (1<<m_switch_left) | (1<<m_switch_right);
}

void PowerController::task_switches()
{
    if(m_switch_port.IN & (1<<m_switch_left))
    {
        m_power_port.DIRSET = (1<<m_power_left);
    }
    else {
        m_power_port.DIRCLR = (1<<m_power_left);
    }


    if(m_switch_port.IN & (1<<m_switch_right))
    {
        m_power_port.DIRSET = (1<<m_power_right);
    }
    else {
        m_power_port.DIRCLR = (1<<m_power_right);
    }
}

void PowerController::power_off()
{
    m_power_port.OUTCLR = (1<<m_power_right);
    m_power_port.OUTCLR = (1<<m_power_left);
}

void PowerController::power_on()
{
     PORTC.OUTSET = (1<<m_power_right);
     PORTC.OUTSET = (1<<m_power_left);
}
