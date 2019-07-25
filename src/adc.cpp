#include "adc.h"

#include "led.h"
extern Led led2;

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

    PORTA.DIRCLR = (1<<PIN1) | (1<<PIN5);

    m_adc.REFCTRL = m_adc.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm;//1V Ref
    m_adc.PRESCALER = ADC_PRESCALER_DIV16_gc; //Prescaler 16
    m_adc.CTRLB = ADC_RESOLUTION_12BIT_gc  | ADC_CONMODE_bm;


    m_adc.CH0.CTRL = ADC_CH_GAIN_DIV2_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    m_adc.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc | ADC_CH_MUXNEG_GND_MODE4_gc;
    m_adc.CH1.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    m_adc.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_GND_MODE4_gc;
    m_adc.CH2.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    m_adc.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN4_gc;
    m_adc.CH3.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    m_adc.CH3.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc | ADC_CH_MUXNEG_PIN5_gc;

    m_adc.CTRLA = ADC_ENABLE_bm;
}




void ADC::start_it()
{

    m_adc.CH3.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;

    m_adc.CTRLA |= ADC_CH0START_bm | ADC_CH1START_bm | ADC_CH2START_bm | ADC_CH3START_bm;

}

void ADC::ch3_interrupt()
{
    //In case ch3 is finished CH0 - CH2 are also finished
    last_results[0] = m_adc.CH0.RES;
    last_results[1] = m_adc.CH1.RES;
    last_results[2] = m_adc.CH2.RES;
    last_results[3] = m_adc.CH3.RES;
    led2.toggle();
}

uint16_t ADC::lastResult(uint8_t index) const
{
    return last_results[index];
}
