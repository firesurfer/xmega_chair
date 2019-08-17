#include "speedcontroller.h"
#include "powermanager.h"
#include "led.h"

extern PowerManager pmanager;
extern Uart uartc0;
extern Led led1;
SpeedController::SpeedController(Uart &uleft, Uart &uright, ADC &apoti):
    uart_left(uleft),
    uart_right(uright),
    adc_poti(apoti),
    parser_left(uleft),
    parser_right(uright)
{
    pid_controller.kP = 12;
    pid_controller.kI = 0;
    pid_controller.kD = 25;
    pid_controller.sum = 0;
    pid_controller.maximum = 350;
    pid_controller.target = 0;
    pid_controller.sum_max = (pid_controller.maximum*64) / pid_controller.kI;

    Uart::rx_handler_t rx_left= reinterpret_cast<Uart::rx_handler_t>(&SpeedController::uart_callback_left);
    Uart::rx_handler_t rx_right= reinterpret_cast<Uart::rx_handler_t>(&SpeedController::uart_callback_right);

    uart_left.set_rx_handler(rx_left,this);
    uart_right.set_rx_handler(rx_right,this);

    UartParser::CommandHandler handler_left = reinterpret_cast<UartParser::CommandHandler>(&SpeedController::parser_callback_left);
    UartParser::CommandHandler handler_right = reinterpret_cast<UartParser::CommandHandler>(&SpeedController::parser_callback_right);

    parser_left.set_command_handler(handler_left,this);
    parser_right.set_command_handler(handler_right,this);
}

void SpeedController::update()
{
    //First -> get angle in 1/100 degree
    int32_t angle = adc_to_angle(adc_poti.lastResult(2));
    //Low pass
    const int32_t filter = 25;
    last_angle = last_angle * (100-filter) + angle * filter;
    last_angle /= 100;
    if(!m_locked)
    {
        uart_counter++;
        if(uart_counter > 500)
        {
            pmanager.lock();
            uart_counter = 0;
        }

        if(abs(last_angle) > 5000)
        {
            pmanager.lock();
            uartc0.transmit_it("Angle power off\n");
            char buffer[5];
            itoa(last_angle, buffer,10);
            uartc0.transmit_it(buffer);
            uartc0.transmit_it("\n");
        }

        //PID Controller
        int16_t diff_speed =  pid_controller.update(last_angle);
        //Rear wheels -> base speed
        speed_left_rear = speed_base;
        speed_right_rear = speed_base;
        //Front wheel -> controlled -> speed offset
        speed_left_front = speed_base +diff_speed;
        speed_right_front = speed_base -diff_speed;

        //Limit to a maximum value -> And do mode specific stuff
        if(drive_mode == DriveMode::FrontSteering)
        {
            speed_left_rear = limit(speed_left_rear, limit_before_sending);
            speed_right_rear = limit(speed_right_rear, limit_before_sending);
            speed_left_front = limit(speed_left_front, limit_before_sending);
            speed_right_front = limit(speed_right_front, limit_before_sending);
        }
        else if(drive_mode == DriveMode::DifferentialDrive)
        {
            pid_controller.target = 0;
            int32_t boost = (angle_set) / 10;
            speed_left_rear = limit(speed_left_rear + boost, limit_before_sending);
            speed_right_rear = limit(speed_right_rear - boost, limit_before_sending);
            speed_left_front = limit(speed_left_front + boost, limit_before_sending);
            speed_right_front = limit(speed_right_front - boost, limit_before_sending);
        }
        else if(drive_mode == DriveMode::CombinedDrive)
        {
            int32_t boost = (pid_controller.target  * speed_base) / 10000;
            speed_left_rear = limit(speed_left_rear + boost, limit_before_sending);
            speed_right_rear = limit(speed_right_rear - boost, limit_before_sending);
            speed_left_front = limit(speed_left_front, limit_before_sending);
            speed_right_front = limit(speed_right_front, limit_before_sending);
        }

        //1 - rear, 2 - front
        //Send and adapt signs
        send_counter++;
        if(!send_mutex){
            if(send_counter >= 0)
            {
                send_mutex=true;
                send_packet(1, -speed_left_rear, uart_left);
                send_packet(2, -speed_left_front, uart_left);

                send_packet(1, speed_right_rear, uart_right);
                send_packet(2, speed_right_front, uart_right);
                send_mutex=false;
                send_counter = 0;
            }
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

    speed_left_rear_m = 0;
    speed_right_rear_m = 0;
    speed_left_front_m = 0;
    speed_right_front_m = 0;
    send_speed_to_pc();
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
    send_packet(2, 0, uart_right);
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
    if(drive_mode == DriveMode::DifferentialDrive){
        pid_controller.target = 0;
        angle_set = angle;
        return;
    }
    pid_controller.target = angle;
}

void SpeedController::set_weak(WheelPosition pos, uint16_t val)
{
    switch (pos) {
    case WheelPosition::FrontLeft:
        send_packet(3,val,uart_left );
        break;
    case WheelPosition::FrontRight:
        send_packet(3,val,uart_right );
        break;
    case WheelPosition::RearLeft:
        send_packet(4,val,uart_left );
        break;
    case WheelPosition::RearRight:
        send_packet(4,val,uart_right );
        break;
    }
}

void SpeedController::set_mode(DriveMode mode)
{
    drive_mode = mode;
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

    int32_t val = (int32_t)adc * 391 - 9001;
    val /= 100;
    return (int16_t)val;
}

void SpeedController::send_speed_to_pc()
{
    send_packet(15, speed_left_front_m,uartc0);
    send_packet(16, speed_left_rear_m,uartc0);
    send_packet(17, speed_right_front_m,uartc0);
    send_packet(18, speed_right_rear_m,uartc0);
    send_packet(19, (int16_t)last_angle,uartc0);
}

int16_t SpeedController::limit(int16_t val, int16_t limit)
{
    if(val > limit)
        val = limit;
    else if(val < -limit)
        val = -limit;
    return val;
}

void SpeedController::uart_callback_left(uint8_t c)
{
    uart_counter = 0;
    parser_left.rx_handler(c);
}

void SpeedController::uart_callback_right(uint8_t c)
{
    uart_counter = 0;
    parser_right.rx_handler(c);
}

void SpeedController::parser_callback_left(uint8_t cmd, uint16_t &data)
{
    if(cmd == 10)
    {   //front
        speed_left_front_m = (int16_t)data;
    }
    else if(cmd == 11)
    {   //rear
        speed_left_rear_m = (int16_t)data;
    }
}

void SpeedController::parser_callback_right(uint8_t cmd, uint16_t &data)
{
    if(cmd == 10)
    {   //front
        speed_right_front_m = -(int16_t)data;
    }
    else if(cmd == 11)
    {   //rear
        led1.toggle();
        speed_right_rear_m = -(int16_t)data;
    }
}
