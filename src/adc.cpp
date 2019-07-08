#include "adc.h"


ADC::ADC(ADC_t &adc):
    m_adc(adc)
{
    uint8_t CalibrationByteL;
    uint8_t CalibrationByteH;
    NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
    CalibrationByteL = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
    CalibrationByteH = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;
    ADCA.CALL = CalibrationByteL;
    ADCA.CALH = CalibrationByteH;

    PORTA.DIRCLR = (1<<PIN1);

    adc.REFCTRL = ADC_REFSEL_INTVCC2_gc; //VCC/2
    adc.PRESCALER = ADC_PRESCALER_DIV16_gc; //Prescaler 16
    adc.CTRLB = ADC_RESOLUTION_12BIT_gc | ADC_CURRLIMIT_LOW_gc | ADC_CONMODE_bm;

    adc.CH0.CTRL = ADC_CH_GAIN_DIV2_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc | ADC_CH_MUXNEG_GND_MODE4_gc;
    adc.CH1.CTRL = ADC_CH_GAIN_DIV2_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_GND_MODE4_gc;
    adc.CH2.CTRL = ADC_CH_GAIN_DIV2_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN4_gc;
    adc.CH3.CTRL = ADC_CH_GAIN_DIV2_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH3.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc | ADC_CH_MUXNEG_PIN5_gc;

    adc.CTRLA = ADC_ENABLE_bm;
}

uint16_t ADC::read()
{
    m_adc.CH1.CTRL |= ADC_CH_START_bm;
    while(!(m_adc.CH1.INTFLAGS & ADC_CH_CHIF_bm));

    m_adc.CH1.INTFLAGS = ADC_CH_CHIF_bm;
    uint16_t result = ADCA.CH1.RES;
    last_result = result;
    return result;

}

void ADC::start_it()
{
    m_adc.CH1.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;
}

void ADC::ch1_interrupt()
{
    uint16_t result = m_adc.CH1.RES;

    last_result = result;
}

uint16_t ADC::lastResult() const
{
    return last_result;
}
