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

/**
 * made by Budali11
 * @name    user_tick.cpp
 * @brief   This file contents functions about systick
 */
#include "user_core.h"
#include "gpio.h"
#include "cs.h"
#include <string.h>
#include "debug.h"

extern "C"
{
    void SysTick_Handler(void);
}
extern User_Nvic_T user_nvic_ctrler;

User_Systick_T user_systick = User_Systick_T(); 

User_Systick_T::User_Systick_T()
{
    m_base = SysTick;
}

User_Systick_T::~User_Systick_T()
{
}


void User_Systick_T::init(void)
{
    m_base->CTRL = 0x04; //configure systick source clock
    m_base->CTRL = 0x02; //enable interrupt


    //wait for additional functions
}

void User_Systick_T::start(void)
{
    uint32_t MCLK = CS_getMCLK();
    uint32_t LOAD_value = 0;
    LOAD_value = MCLK / 1000 * m_period_ms;
    for(m_times = 0; LOAD_value > 0xFFFFFF; m_times++)
        LOAD_value /= 2;
    m_tmp = m_times;
    m_base->LOAD = LOAD_value; //set up period
    m_base->VAL = 0x00; //write any value to VAL
    m_base->CTRL |= 0x01; //enable systick
}

void User_Systick_T::stop(void)
{
    m_base->CTRL &= ~(1 << 0); //disable systick
}

void User_Systick_T::set_period(uint32_t ms)
{
    m_period_ms = ms;
    m_times = 0;
}

void User_Systick_T::set_interrupt(uint32_t priority, bool status)
{
    /*configure interrupt*/
    user_nvic_ctrler.set_up_IRQn(SysTick_IRQn, priority, status);
}

void User_Systick_T::handler(void)
{
    if(m_tmp == 0)
        {GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); m_tmp = m_times;}
    else
        m_tmp--;
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    user_systick.handler();
}
