#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

class PidController{
public:
    PidController();//the constructor could get the PID params from the eeprom. the setter of kP,kI,kD could store the values in eeprom
    int16_t update(int32_t value);

    int32_t sum;
    int32_t last_diff;
    int32_t target;
    int32_t kP;
    int32_t kD;
    int32_t kI;
    int16_t maximum = 1000;
    int32_t sum_max;
};
