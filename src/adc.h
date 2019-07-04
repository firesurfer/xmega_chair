#pragma once


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include <stddef.h>

class ADC
{
public:
    ADC(ADC_t & adc);
    uint16_t read();
    void start_it();
    void ch1_interrupt();
    uint16_t lastResult() const;

private:
    ADC_t m_adc;
    uint16_t last_result;
};
