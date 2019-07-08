#ifndef SPEEDCONTROLLER_H
#define SPEEDCONTROLLER_H

#include "pid_controller.h"
#include "uart.h"
#include "adc.h"

class SpeedController
{
public:
    SpeedController(Uart& uleft, Uart& uright, ADC& apoti);
    void update();
    void set_speed(int16_t speed);
    void set_angle(int16_t angle);
private:
    Uart& uart_left;
    Uart& uart_right;
    ADC& adc_poti;

    void send_packet(uint8_t command, uint16_t data, Uart &uart);

    int16_t adc_to_angle(int16_t adc);
    pid_controller::pid_data pid_controller;

    int16_t speed_left_front;
    int16_t speed_left_rear;
    int16_t speed_right_front;
    int16_t speed_right_rear;
    int16_t speed_base;
};

#endif // SPEEDCONTROLLER_H
