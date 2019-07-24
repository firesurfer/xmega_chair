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

    adc.REFCTRL = ADC_REFSEL_INT1V_gc; //1V Ref
    adc.PRESCALER = ADC_PRESCALER_DIV16_gc; //Prescaler 16
    adc.CTRLB = ADC_RESOLUTION_12BIT_gc  | ADC_CONMODE_bm;


    adc.CH0.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN2_gc | ADC_CH_MUXNEG_PIN5_gc;
    adc.CH1.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH1.MUXCTRL = ADC_CH_MUXPOS_PIN6_gc | ADC_CH_MUXNEG_GND_MODE4_gc;
    adc.CH2.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH2.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc | ADC_CH_MUXNEG_PIN4_gc;
    adc.CH3.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_DIFFWGAIN_gc;
    adc.CH3.MUXCTRL = ADC_CH_MUXPOS_PIN3_gc | ADC_CH_MUXNEG_PIN5_gc;

    adc.CTRLA = ADC_ENABLE_bm;
}



void ADC::start_it()
{
    m_adc.CH1.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_LO_gc;
}

void ADC::ch3_interrupt()
{
    last_results[0] = m_adc.CH0.RES;
    last_results[0] = m_adc.CH1.RES;
    last_results[0] = m_adc.CH2.RES;
    last_results[0]= m_adc.CH3.RES;
    led2.toggle();


}

uint16_t ADC::lastResult(uint8_t index) const
{
    return last_results[index];
}
