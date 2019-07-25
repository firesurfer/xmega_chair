#ifndef SPEEDCONTROLLER_H
#define SPEEDCONTROLLER_H

#include "pid_controller.h"
#include "uart.h"
#include "adc.h"

class SpeedController
{
public:
    SpeedController(Uart& uleft, Uart& uright, ADC& apoti);
    /**
     * @brief update - run pid controller in order to control front angle
     */
    void update();
    /**
     * @brief lock the controller -> set speed to 0
     */
    void lock();
    /**
     * @brief unlock the controller
     */
    void unlock();
    /**
     * @brief set_speed
     * @param speed - base speed
     */
    void set_speed(int16_t speed);
    /**
     * @brief set_angle
     * @param angle - desired front angle
     */
    void set_angle(int16_t angle);
    /**
     * @brief send_packet - send data packet in style: | # | cmd | high data | low data | checksum |
     * @param command
     * @param data
     * @param uart
     */
    void send_packet(uint8_t command, uint16_t data, Uart &uart);
    /**
     * @brief adc_to_angle - convert adc value to angle in 1/100 degree - fixed comma arithmethic
     * @param adc
     * @return
     */
    static int16_t adc_to_angle(int16_t adc);

    PidController pid_controller;
private:
    Uart& uart_left;
    Uart& uart_right;
    ADC& adc_poti;

    int16_t speed_left_front;
    int16_t speed_left_rear;
    int16_t speed_right_front;
    int16_t speed_right_rear;
    int16_t speed_base;
    int16_t last_angle = 0;

    /*Limits*/
    const int16_t speed_limit = 1000;
    const int16_t angle_limit = 4500;
    const int16_t limit_before_sending = 1000;

    bool m_locked = true;
    int16_t limit(int16_t val, int16_t limit);
};

#endif // SPEEDCONTROLLER_H
