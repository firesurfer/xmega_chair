#pragma once



#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
namespace pid_controller {


typedef struct
{
    int32_t sum;
    int32_t last_diff;
    int32_t target;
    int32_t kP;
    int32_t kD;
    int32_t kI;
    int16_t maximum;
    int32_t sum_max;
}pid_data;



int16_t update(int32_t value, pid_data* data);

int16_t ensure_max(int32_t update, bool * reached, pid_data* data);

}



