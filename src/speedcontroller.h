#ifndef SPEEDCONTROLLER_H
#define SPEEDCONTROLLER_H

#include "pid_controller.h"
#include "uart.h"
#include "adc.h"

#include <math.h>

enum class WheelPosition
{
    FrontLeft = 0,
    FrontRight = 1,
    RearLeft = 2,
    RearRight = 3
};
enum class DriveMode
{
    FrontSteering = 0,
    DifferentialDrive = 1,
    CombinedDrive = 2
};

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
     * @brief set_weak - set field weakening of specified motor
     * @param pos
     * @param val
     */
    void set_weak(WheelPosition pos, uint16_t val);
    /**
     * @brief set_mode - sets the drive mode
     * @param mode
     */
    void set_mode(DriveMode mode);
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
    int32_t last_angle = 0;

    volatile DriveMode drive_mode = DriveMode::FrontSteering;

    /*Limits*/
    const int16_t speed_limit = 1000;
    const int16_t angle_limit = 4200;
    const int16_t limit_before_sending = 1000;

    bool m_locked = true;
    volatile bool send_mutex=false;
    int16_t limit(int16_t val, int16_t limit);

    void uart_callback_left(uint8_t c);
    void uart_callback_right(uint8_t c);

    uint16_t uart_counter = 0;
    uint8_t send_counter = 0;
};

#endif // SPEEDCONTROLLER_H
