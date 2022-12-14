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
 * @name    user_uart.cpp
 * @brief   This file contents functions about uart
 */
#include "msp.h"
#include "gpio.h"
#include "user_uart.h"
#include <debug.h>

#define DEBUG

void User_Uart_T::m_send_Nchar(uint8_t *str, uint32_t n)
{
    char *pstr = (char *)str;
    for(uint32_t i = 0; i < n; i++)
    {
        m_base->TXBUF = *pstr;
        pstr++;
        while(m_base->STATW & (1 << 0));
    }
}

User_Uart_T::User_Uart_T(EUSCI_A_Type *base, uint32_t baudrate)
{
    //base and baudrate must be one of these value
    ASSERT(base == EUSCI_A0 || \
        base == EUSCI_A1 || \
        base == EUSCI_A2 || \
        base == EUSCI_A3);
    ASSERT(baudrate == 9600 || \
        baudrate == 19200 || \
        baudrate == 38400 || \
        baudrate == 57600 || \
        baudrate == 115200 || \
        baudrate == 230400 || \
        baudrate == 460800);
    m_base = base;
    m_baudrate = baudrate;
}

User_Uart_T::~User_Uart_T()
{
}

void User_Uart_T::init(EUSCI_A_Type *base, uint32_t baudrate)
{
    m_base = base;
    m_baudrate = baudrate;
    //set UCSWRST
    m_base->CTLW0 = 0x01;
    //init all register
    //select SMCLK as uart clk
    m_base->CTLW0 |= 0xC0;
    m_base->CTLW1 = 0x00;
    //set UCBRx
    //set UCOS16, UCBRFx, UCBRSx
    //setup baudrate
    switch (m_baudrate)
    {
    case 9600:
        m_base->BRW = 0x4E;
        m_base->MCTLW = BAUDRATE_9600;
        break;
    case 19200:
        m_base->BRW = 0x27;
        m_base->MCTLW = BAUDRATE_19200;
        break;
    case 38400:
        m_base->BRW = 0x13;
        m_base->MCTLW = BAUDRATE_38400;
        break;
    case 57600:
        m_base->BRW = 0x0D;
        m_base->MCTLW = BAUDRATE_57600;
        break;
    case 115200:
        m_base->BRW = 0x6;
        m_base->MCTLW = BAUDRATE_115200;
        break;
    case 230400:
        m_base->BRW = 0x3;
        m_base->MCTLW = BAUDRATE_230400;
        break;
    case 460800:
        m_base->BRW = 0x1;
        m_base->MCTLW = BAUDRATE_460800;
        break;
    default:
        break;
    }
    //disable auto baudrate
    m_base->ABCTL = 0x00;
    //disable IrDA encode and decode
    m_base->IRCTL = 0x00;
    //configure ports
    GPIO_setAsPeripheralModuleFunctionOutputPin(UART_TXD_PORT, UART_TXD_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(UART_RXD_PORT, UART_RXD_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

    //clear UCSWRST with software
    m_base->CTLW0 &= ~(1 << 0);
    //enable receive interrupt
    m_base->IE = 0x01;
}

void User_Uart_T::send_string(const char *str)
{
    char *pstr = (char *)str;
    while((*pstr) != '\0')
    {
        m_base->TXBUF = *pstr;
        pstr++;
        while(m_base->STATW & (1 << 0));
    }
}

void User_Uart_T::send_string(uint8_t *str)
{
    char *pstr = (char *)str;
    while((*pstr) != '\0')
    {
        m_base->TXBUF = *pstr;
        pstr++;
        while(m_base->STATW & (1 << 0));
    }
}

uint32_t User_Uart_T::receive_string(uint8_t *tmp)
{
    uint32_t i = 0;
    for(i = 0; m_base->RXBUF != '\0'; i++);
    {
        while(((m_base->IFG >> 0) & 0x1) != 1);    
        tmp[i] = m_base->RXBUF;
    }
    return i;
}

void User_Uart_T::printf(const char *str, ...)
{
    va_list arg_list;
    size_t i = 0;
    char *pstr = (char *)str;
    va_start(arg_list, str);
    for (uint8_t err = 0; pstr[i] != '\0'; i++)
    {
        if(pstr[i] == '%')
        {
            m_send_Nchar((uint8_t *)pstr, i);
            pstr += i;
            i = 0;
            switch (pstr[1])
            {
                static char tmp[12] = {0};
                case 'd':{
                    int input = va_arg(arg_list, int);
                    sprintf(tmp, "%d", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 'x':{
                    unsigned int input = va_arg(arg_list, unsigned int);
                    sprintf(tmp, "0x%x", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 'u':{
                    uint32_t input = va_arg(arg_list, uint32_t);
                    sprintf(tmp, "%lu", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 'f':{
                    double input = va_arg(arg_list, double);
                    sprintf(tmp, "%f", input);
                    send_string((uint8_t *)tmp);
                }break;
                case 's':{
                    uint8_t *input = va_arg(arg_list, uint8_t *);
                    send_string(input);
                }break;
                case 'c':{
                    char input = va_arg(arg_list, int);
                    m_send_Nchar((uint8_t *)&input, 1);
                }break;
                default:{
                    send_string("suggest to check up the input.\r\n");
                }
            }
            pstr += 2; //now pstr point at two char behind '%'
        }
    }
    va_end(arg_list);
    send_string((uint8_t *)pstr);
}
