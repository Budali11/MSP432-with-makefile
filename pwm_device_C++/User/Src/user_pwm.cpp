/* 
 * user_pwm.cpp
 *
 * Copyright (c) 2022/8/3, Budali11
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022/8/3       Budali11     the first version
 * 2022/8/10      Budali11     transfer file to cpp
 * 
 */
#include "user_pwm.h"
#include "user_uart.h"
#include "timer_a.h"
#include "gpio.h"
#include "debug.h"

extern User_Uart_T yuki;

/**
 * @brief   constructor to specify a timer to use as a pwm device
 * @param   base base address of Timer_Ax
 * @note    don't use the unit_0, because Timer_Ax.0 cannot use as a pwm device
 * @retval  not return 
 */
User_PWM_T::User_PWM_T(Timer_A_Type *base)
{
    ASSERT(base == TIMER_A0 || \
            base == TIMER_A1 || \
            base == TIMER_A2 || \
            base == TIMER_A3); 

    m_base = base;
}

User_PWM_T::~User_PWM_T()
{
    //stop mode
    m_base->CTL &= ~(0b11 << 4);
    //disable interrupt
    m_base->CTL &= ~(1 << 1);
    //clear clock divider
    m_base->CTL &= ~0xC0;
    m_base->EX0 = 0;
    //clear counter value
    m_base->CTL |= (1 << 2);
}

/**
 * @brief   Initialize hardware, configure clock source and setup output mode
 * @param   unit_n you can specify more than 1 unit by using '|'.
 *          this parameter can be some of the following:
 *          - \b TIMER_UNIT_1
 *          - \b TIMER_UNIT_2
 *          - \b TIMER_UNIT_3
 *          - \b TIMER_UNIT_4
 * @note    The precondition for using this function is that had defined a class through constructor
 * @retval  not return 
 */
void User_PWM_T::init(uint8_t unit_n)
{
    for (uint8_t i = 1; i < 5; i++) //i start from 1, skip unit0
    {
        if((unit_n >> i) & 0x1)
            m_unit[i] = 1;
    }
    //configure clock
    //TASSEL
    m_base->CTL |= TIMER_A_CLOCKSOURCE_SMCLK;
    //ID
    m_base->CTL &= ~0xC0; //Divide by 1
    //IDEX
    m_base->EX0 = 0x20; //Divide by 3
    //set TACLR
    m_base->CTL |= (1 << 2);
    
    //configure PWM output mode
    for (uint8_t i = 1; i < 5; i++) //i start from 1, skip unit0
    {
        if(m_unit[i] == 1)
        {
            //set compare mode to generate PWM
            m_base->CCTL[i] &= ~0xC100;
            //Reset/set mode
            m_base->CCTL[i] |= 0xE0;
        }
    }
    //configure port
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, unit_n << 3, GPIO_PRIMARY_MODULE_FUNCTION);
}

/**
 * @brief   Generate PWM signals 
 * @note    must use init() before using this funtion
 * 
 * @retval  not return 
 */
void User_PWM_T::generate()
{
    if (m_base->CCR[0] <= 0)
    {
        yuki.send_string("Please setup CCR0 value first.\r\n");
        return;
    }
    
    //clear TACLR
    m_base->CTL &= ~(1 << 2);
    //select counter mode
    m_base->CTL |= 0x10;
    m_base->CTL &= ~0x20;

}

/**
 * @brief   setup parameters of a pwm device 
 * @param   device a pwm_device pointer point to user's 
 * @param   nHz frequency of the final output signal with Hz
 * @param   percent the high voltage in one period, if you want 40%, then pass 40 into
 * @param   unit_n you can specify more than 1 unit by using '|'.
 *          this parameter can be some of the following:
 *          - \b TIMER_UNIT_1
 *          - \b TIMER_UNIT_2
 *          - \b TIMER_UNIT_3
 *          - \b TIMER_UNIT_4
 *          - \b TIMER_UNIT_5
 *          - \b TIMER_UNIT_6
 * @note    The precondition for using this function is that function PWM_Device_Init has been used
 * 
 * @retval  not return 
 */
void User_PWM_T::setPWM(uint32_t nHz, uint32_t percent, uint8_t unit_n)
{
    //input == 4,000,000Hz, require 400Hz, compute that TACCR0 = 10,000
    if(nHz == 0)
        {yuki.send_string("Bad input nHz.\r\n"); return;}
    uint32_t CCR0_value = 4000000 / nHz;
    uint32_t CCRx_value = CCR0_value / 100 * percent;

    m_base->CCR[0] = CCR0_value;
    for (uint8_t i = 1; i < 6; i++)
    {
        if((unit_n >> i) & 0x1)
            m_base->CCR[i] = CCRx_value;
    }
    
    m_base->R = 0;

}

