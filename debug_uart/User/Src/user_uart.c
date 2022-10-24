/**
 * Copyright 2022 Budali11
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
 */
#include "user_uart.h"

/* preInit function */
void UART_PreInit(void)
{
    //set UCSWRST
    DEBUG_UART->CTLW0 = 0x01;
    //init all register
    //select SMCLK as uart clk
    DEBUG_UART->CTLW0 |= 0xC0;
    DEBUG_UART->CTLW1 = 0x00;
    //set UCBRx
    //set UCOS16, UCBRFx, UCBRSx
    //setup baudrate
    switch (DEBUG_UART_BAUDRATE)
    {
    case 9600:
        DEBUG_UART->BRW = 0x4E;
        DEBUG_UART->MCTLW = BAUDRATE_9600;
        break;
    case 19200:
        DEBUG_UART->BRW = 0x27;
        DEBUG_UART->MCTLW = BAUDRATE_19200;
        break;
    case 38400:
        DEBUG_UART->BRW = 0x13;
        DEBUG_UART->MCTLW = BAUDRATE_38400;
        break;
    case 57600:
        DEBUG_UART->BRW = 0x0D;
        DEBUG_UART->MCTLW = BAUDRATE_57600;
        break;
    case 115200:
        DEBUG_UART->BRW = 0x6;
        DEBUG_UART->MCTLW = BAUDRATE_115200;
        break;
    case 230400:
        DEBUG_UART->BRW = 0x3;
        DEBUG_UART->MCTLW = BAUDRATE_230400;
        break;
    case 460800:
        DEBUG_UART->BRW = 0x1;
        DEBUG_UART->MCTLW = BAUDRATE_460800;
        break;
    default:
        break;
    }
    //disable auto baudrate
    DEBUG_UART->ABCTL = 0x00;
    //disable IrDA encode and decode
    DEBUG_UART->IRCTL = 0x00;
    //configure ports
    GPIO_setAsPeripheralModuleFunctionOutputPin(UART_TXD_PORT, UART_TXD_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(UART_RXD_PORT, UART_RXD_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

    //clear UCSWRST with software
    DEBUG_UART->CTLW0 &= ~(1 << 0);
    //enable receive interrupt
    DEBUG_UART->IE = 0x01;
}

/* private function */
void Send_Nchar(uint8_t *str, uint32_t n)
{
    uint8_t *pstr = str;
    for(uint32_t i = 0; i < n; i++)
    {
        DEBUG_UART->TXBUF = *pstr;
        pstr++;
        while(DEBUG_UART->STATW & (1 << 0));
    }
}

/* public function */
void Send_String(uint8_t *str)
{
    char *pstr = (char *)str;
    while((*pstr) != '\0')
    {
        DEBUG_UART->TXBUF = *pstr;
        pstr++;
        while(DEBUG_UART->STATW & (1 << 0));
    }
}

int Printf(const char *str, ...)
{
    va_list arg_list;
    size_t i = 0;
    char *pstr = (char *)str;
    va_start(arg_list, str);
    for (uint8_t err = 0; pstr[i] != '\0'; i++)
    {
        if(pstr[i] == '%')
        {
            Send_Nchar((uint8_t *)pstr, i);
            pstr += i;
            i = 0;
            switch (pstr[1])
            {
                static char tmp[12] = {0};
                case 'd':{
                    int input = va_arg(arg_list, int);
                    sprintf(tmp, "%d", input);
                    Send_String((uint8_t *)tmp);
                }break;
                case 'x':{
                    unsigned int input = va_arg(arg_list, unsigned int);
                    sprintf(tmp, "0x%x", input);
                    Send_String((uint8_t *)tmp);
                }break;
                case 'u':{
                    uint32_t input = va_arg(arg_list, uint32_t);
                    sprintf(tmp, "%lu", input);
                    Send_String((uint8_t *)tmp);
                }break;
                case 'f':{
                    double input = va_arg(arg_list, double);
                    sprintf(tmp, "%f", input);
                    Send_String((uint8_t *)tmp);
                }break;
                case 's':{
                    uint8_t *input = va_arg(arg_list, uint8_t *);
                    Send_String(input);
                }break;
                case 'c':{
                    char input = va_arg(arg_list, int);
                    Send_Nchar((uint8_t *)&input, 1);
                }break;
                default:{
                    Send_String("suggest to check up the input.\r\n");
                }
            }
            pstr += 2; //now pstr point at two char behind '%'
        }
    }
    va_end(arg_list);
    Send_String((uint8_t *)pstr);
}

int Receive(uint8_t *buf)
{

}

