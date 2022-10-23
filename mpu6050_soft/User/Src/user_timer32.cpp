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

    /* periodic mode */
    m_base->CONTROL |= 1 << 6;

    /* disable interrupt */
    m_base->CONTROL &= ~(1 << 5);

    /* select clock source divider(divided by 16) */
    m_base->CONTROL |= 1 << 2;
    m_base->CONTROL &= ~(1 << 3);

    /* select as 16-bit counter */
    m_base->CONTROL &= ~(1 << 1);

    /* select as one shot mode */
    m_base->CONTROL |= 1 << 0;

    /* load vlaue */
    /* set 16-bit value, save computing time */
    m_base->LOAD = 0xffff;

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

void clock::clock_init(void)
{    
    uint32_t MCLK = CS_getMCLK();
    m_us = (float)(MCLK / 16) / 1000000.0;
    m_load = 0xffff;
    
    start();

    while(read_timer() != 0);
}

void clock::operator=(uint32_t us)
{
    m_load = (us == 0) ? 0xffff : us;
    load(m_load);
}

void clock::operator>>(float & rtime)
{
    uint32_t timer_value = m_load - read_timer();
    // yuki.printf("tim_val: %d ", timer_value);

    rtime = (timer_value == 0) ? -1 : ((float)timer_value / m_us);
}