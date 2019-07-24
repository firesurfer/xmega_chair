#include "speedcontroller.h"

SpeedController::SpeedController(Uart &uleft, Uart &uright, ADC &apoti):
    uart_left(uleft),
    uart_right(uright),
    adc_poti(apoti)
{
    pid_controller.kP = 5;
    pid_controller.kI = 0;
    pid_controller.kD = 0;
    pid_controller.sum = 0;
    pid_controller.maximum = 1000;
    pid_controller.target = 0;
}

void SpeedController::update()
{
    if(!m_locked)
    {
        int16_t angle = adc_to_angle(adc_poti.lastResult(2));
        last_angle += angle;
        last_angle /= 2;

        int16_t diff_speed =  pid_controller.update(last_angle);
        speed_left_rear = speed_base;
        speed_right_rear = speed_base;
        speed_left_front = speed_base -diff_speed;
        speed_right_front = speed_base +diff_speed;

        //1 - rear, 2 - front
        send_packet(1,speed_left_rear, uart_left);
        send_packet(2, -speed_left_front, uart_left);

        send_packet(1, -speed_right_rear, uart_right);
        send_packet(2, speed_right_front, uart_right);
    }
}

void SpeedController::lock()
{
    m_locked = true;

    send_packet(1,0, uart_left);
    send_packet(2, 0, uart_left);

    send_packet(1, 0, uart_right);
    send_packet(2, 0, uart_right);
}

void SpeedController::unlock()
{
    m_locked = false;
}

void SpeedController::set_speed(int16_t speed)
{
    if(speed > limit)
        speed = limit;
    if(speed < -limit)
        speed = -limit;
    speed_base = speed;
}

void SpeedController::set_angle(int16_t angle)
{
    if(angle > 4500)
    {
        angle = 4500;
    }
    else if(angle < -4500)
    {
        angle = -4500;
    }
    pid_controller.target = angle;
}

void SpeedController::send_packet(uint8_t command, uint16_t data, Uart& uart)
{
    uint8_t buffer[5];
    buffer[0] = '#';
    buffer[1] = command;
    buffer[2] = (data & 0xFF00) >> 8;
    buffer[3] = data & 0xFF;

    uint8_t checksum = buffer[0];
    for(uint8_t i = 1; i < 4;i++)
    {
        checksum ^= buffer[i];
    }
    buffer[4] = checksum;

    uart.transmit_it(buffer,5);
}

int16_t SpeedController::adc_to_angle(int16_t adc)
{   //m = 0,03916449086161879896
    //c = -0,90078328981723237598

    int32_t val = (int32_t)adc * 391 - 9007;
    val /= 100;
    return (int16_t)val;
}
