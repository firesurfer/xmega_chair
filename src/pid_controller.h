#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

//template<typename kP, typename kD, typename kI>
class PidController{
public:
    PidController();//the constructor could get the PID params from the eeprom. the setter of kP,kI,kD could store the values in eeprom
    int16_t update(int16_t value);

    int16_t sum;
    int16_t last_diff;
    int16_t target;
    uint16_t kP;
    uint16_t kD;
    uint16_t kI;
    int16_t maximum = 350;
    int16_t sum_max;
};
