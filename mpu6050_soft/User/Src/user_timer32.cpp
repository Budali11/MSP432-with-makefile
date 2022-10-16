// Copyright 2022 Budali11
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "user_timer32.h"
#include "cs.h"
#include "user_uart.h"

extern User_Uart_T yuki;

User_Timer32_T::User_Timer32_T(/* args */)
{
}

User_Timer32_T::~User_Timer32_T()
{
}

void User_Timer32_T::init(Timer32_Type *base, uint8_t mode)
{
    m_base = base;

    /* disable timer */
    m_base->CONTROL &= ~(1 << 7);

    /* free-running mode */
    m_base->CONTROL &= ~(1 << 6);

    /* disable interrupt */
    m_base->CONTROL &= ~(1 << 5);

    /* select clock source divider(divided by 16) */
    m_base->CONTROL |= 1 << 2;
    m_base->CONTROL &= ~(1 << 3);

    /* select as 16-bit counter */
    m_base->CONTROL &= ~(1 << 1);

    /* select as one shot mode */
    m_base->CONTROL |= 1 << 0;

}

/**********************clock class*********************************/
clock::clock()
{
}

clock::clock(uint8_t timer_n)
{
    if(timer_n == TIMER_1)
        init(TIMER32_1, 0);
    else if(timer_n == TIMER_2)
        init(TIMER32_2, 0);
    else
        yuki.printf("clock init error.\r\n");
}

clock::~clock()
{
}

void clock::operator=(uint32_t us)
{
    if(us == 0)
        start();

}

float clock::operator>>(float & rtime)
{
    stop();

    m_timer_value = read_timer();
    if(m_timer_value == 0)
    {
        rtime = -1;
        return -1;
    }

    uint32_t MCLK = CS_getMCLK();
    float us = (MCLK / 16.0f) / 1000000.0f;

    rtime = m_timer_value/us;
    return m_timer_value/us;
}