#include "speedcontroller.h"
#include "powermanager.h"

extern PowerManager pmanager;
SpeedController::SpeedController(Uart &uleft, Uart &uright, ADC &apoti):
    uart_left(uleft),
    uart_right(uright),
    adc_poti(apoti)
{
    pid_controller.kP = 20;
    pid_controller.kI = 20;
    pid_controller.kD = 8;
    pid_controller.sum = 0;
    pid_controller.maximum = 1000;
    pid_controller.target = 0;
    pid_controller.sum_max = (pid_controller.maximum*64) / pid_controller.kI;

    Uart::rx_handler_t rx= reinterpret_cast<Uart::rx_handler_t>(&SpeedController::uart_callback);
    uart_left.set_rx_handler(rx,this);
    uart_right.set_rx_handler(rx,this);
}

void SpeedController::update()
{
    if(!m_locked)
    {
        uart_counter++;
        if(uart_counter > 500)
        {
            //TODO disable power
            pmanager.off();
        }
        //First -> get angle in 1/100 degree
        int16_t angle = adc_to_angle(adc_poti.lastResult(2));
        //Low pass
        last_angle += angle;
        last_angle /= 2;

        //PID Controller
        int16_t diff_speed =  pid_controller.update(last_angle);
        //Rear wheels -> base speed
        speed_left_rear = speed_base;
        speed_right_rear = speed_base;
        //Front wheel -> controlled -> speed offset
        speed_left_front = speed_base +diff_speed;
        speed_right_front = speed_base -diff_speed;

        //Limit to a maximum value
        speed_left_rear = limit(speed_left_rear, limit_before_sending);
        speed_right_rear = limit(speed_right_rear, limit_before_sending);
        speed_left_front = limit(speed_left_front, limit_before_sending);
        speed_right_front = limit(speed_right_front, limit_before_sending);

        //1 - rear, 2 - front
        //Send and adapt signs
        if(!send_mutex){
            send_mutex=true;
            send_packet(1,speed_left_rear, uart_left);
            send_packet(2, -speed_left_front, uart_left);

            send_packet(1, -speed_right_rear, uart_right);
            send_packet(2, speed_right_front, uart_right);
            send_mutex=false;
        }
    }

}

void SpeedController::lock()
{
    m_locked = true;

    send_mutex=true;
    send_packet(1,0, uart_left);
    send_packet(2, 0, uart_left);

    send_packet(1, 0, uart_right);
    send_packet(2, 0, uart_right);
    send_mutex=false;
}

void SpeedController::unlock()
{
    m_locked = false;
}

void SpeedController::set_speed(int16_t speed)
{
    if(speed > speed_limit)
        speed = speed_limit;
    if(speed < -speed_limit)
        speed = -speed_limit;
    speed_base = speed;
}

void SpeedController::set_angle(int16_t angle)
{
    if(angle > angle_limit)
    {
        angle = angle_limit;
    }
    else if(angle < -angle_limit)
    {
        angle = -angle_limit;
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

    //Calculate checksum with xor
    uint8_t checksum = buffer[0];
    for(uint8_t i = 1; i < 4;i++)
    {
        checksum ^= buffer[i];
    }
    buffer[4] = checksum;

    //Transmit -> interrupt based
    uart.transmit_it(buffer,5);
}

int16_t SpeedController::adc_to_angle(int16_t adc)
{   //m = 0,03916449086161879896
    //c = -0,90078328981723237598

    int32_t val = (int32_t)adc * 391;
    val /= 100;
    return (int16_t)val;
}

int16_t SpeedController::limit(int16_t val, int16_t limit)
{
    if(val > limit)
        val = limit;
    else if(val < -limit)
        val = -limit;
    return val;
}

void SpeedController::uart_callback(uint8_t c)
{
    uart_counter = 0;
}
