#include "powercontroller.h"

PowerController::PowerController()
{
    PORTC.DIRSET = (1<<m_power_left) | ( 1<< m_power_right);
    PORTC.OUTCLR = (1<<m_power_left) | ( 1<< m_power_right);
    m_switch_port.DIRCLR = (1<<m_switch_left) | (1<<m_switch_right);
    m_power_on = false;
}

void PowerController::task_switches()
{
    if(m_switch_port.IN & (1<<m_switch_left))
    {
        if(m_power_on)
            m_power_port.OUTSET = (1<<m_power_left);
    }
    else {
        m_power_port.OUTCLR = (1<<m_power_left);
    }


    if(m_switch_port.IN & (1<<m_switch_right))
    {
        if(m_power_on)
            m_power_port.OUTSET = (1<<m_power_right);
    }
    else {
        m_power_port.OUTCLR = (1<<m_power_right);
    }
}

void PowerController::off()
{
    m_power_port.OUTCLR = (1<<m_power_right);
    m_power_port.OUTCLR = (1<<m_power_left);
}

void PowerController::on()
{
    if(m_power_on)
    {
        m_power_port.OUTSET = (1<<m_power_right);
        m_power_port.OUTSET = (1<<m_power_left);
    }
}

void PowerController::lock()
{
    m_power_on = false;
    off();
}

void PowerController::unlock()
{
    m_power_on = true;
}
