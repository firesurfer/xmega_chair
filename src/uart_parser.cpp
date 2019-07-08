/*
 * Copyright 2017 <Lennart Nachtigall, Friedolin Gröger> <firesurfer127@gmail.com, gfrodo@gmx.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "uart_parser.h"

UartParser::UartParser(Uart &uart):
    m_uart(uart)
{

}

void UartParser::rx_handler(char c)
{
    static uint8_t buf[5];
    static uint8_t index = 0;

    switch (index) {
    case 0:
        if (c == '#')++index;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        buf[index] = c;
        ++index;
        if(index==5){
            uint8_t checksum = '#';
            for(int i = 1; i < 4;i++)
            {
                checksum ^= buf[i];
            }
            if(checksum == buf[4])
            {
                uint16_t data = buf[3] | (uint16_t)buf[2] << 8;
                m_command_handler(buf[1],data);
            }
            index = 0;
        }
        break;
    default:
        index = 0;
    }
}

void UartParser::set_command_handler(CommandHandler func)
{
     m_command_handler = func;
}
