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

#pragma once

#include "uart.h"
#include <stdbool.h>
#include "uart.h"

class UartParser
{
public:
    typedef  void (*CommandHandler)(void*,uint8_t, uint16_t&);

    UartParser(Uart& uart);
    void rx_handler(char c);
    void send_command(uint8_t command, uint16_t data);

    void set_command_handler(CommandHandler func, void* obj);
private:
    Uart& m_uart;
    CommandHandler m_command_handler = nullptr;
    void * m_command_handler_obj = nullptr;
    uint8_t buf[5];
    uint8_t index = 0;
};


