#include "powermanager.h"
#include <util/delay.h>
#include "led.h"

extern Led led3;
extern Led led4;
PowerManager::PowerManager()
{
    PORTC.DIRSET = (1<<m_power_left) | ( 1<< m_power_right);
    PORTC.OUTCLR = (1<<m_power_left) | ( 1<< m_power_right);
    m_switch_port.DIRCLR = (1<<m_switch_left) | (1<<m_switch_right);
    m_switch_port.PIN0CTRL = PORT_OPC_PULLUP_gc;
    m_switch_port.PIN1CTRL = PORT_OPC_PULLUP_gc;
    m_power_on = false;
}

void PowerManager::task_switches()
{
    if(m_power_on)
    {
        if(m_switch_port.IN & (1<<m_switch_left))
        {

            _delay_ms(10);
            if(m_switch_port.IN & (1<<m_switch_left))
            {
                if(m_power_on)
                {
                    led3.clear();
                    m_power_port.OUTCLR = (1<<m_power_left);

                }
            }

        }
        else {

            led3.set();
            m_power_port.OUTSET = (1<<m_power_left);
        }

        if(m_switch_port.IN & (1<<m_switch_right))
        {

            _delay_ms(10);
            if(m_switch_port.IN & (1<<m_switch_right))
            {
                if(m_power_on)
                {
                    led4.clear();
                    m_power_port.OUTCLR = (1<<m_power_right);

                }
            }

        }
        else {
            m_power_port.OUTSET = (1<<m_power_right);
            led4.set();
        }
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
