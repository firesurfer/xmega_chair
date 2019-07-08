#include "powermanager.h"

PowerManager::PowerManager()
{
    PORTC.DIRSET = (1<<m_power_left) | ( 1<< m_power_right);
    PORTC.OUTCLR = (1<<m_power_left) | ( 1<< m_power_right);
    m_switch_port.DIRCLR = (1<<m_switch_left) | (1<<m_switch_right);
    m_power_on = false;
}

void PowerManager::task_switches()
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

void PowerManager::off()
{
    m_power_port.OUTCLR = (1<<m_power_right);
    m_power_port.OUTCLR = (1<<m_power_left);
}

void PowerManager::on()
{
    if(m_power_on)
    {
        m_power_port.OUTSET = (1<<m_power_right);
        m_power_port.OUTSET = (1<<m_power_left);
    }
}

void PowerManager::lock()
{
    m_power_on = false;
    off();
}

void PowerManager::unlock()
{
    m_power_on = true;
}
