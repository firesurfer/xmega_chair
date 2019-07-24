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

    void start_it();

    void ch3_interrupt();
    uint16_t lastResult(uint8_t index) const;

private:
    ADC_t m_adc;
    uint16_t last_results[4];
};
