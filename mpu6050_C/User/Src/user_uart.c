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
#include "user_periph.h"
#include "user_uart.h"
#include "user_dma.h"
#include "interrupt.h"

extern uint32_t data_control_structure[][4];
static char txbuffer[MAX_TRANS_BYTES];

/* preInit function */
static int UART_PreInit(void)
{
    int ret = 0;
    //set UCSWRST
    DEBUG_UART->CTLW0 = 0x01;
    //init all register
    //select SMCLK as uart clk
    DEBUG_UART->CTLW0 |= 0xC0;
    DEBUG_UART->CTLW1 = 0x03;
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
    ret = -1;
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

    /* assign interrupt */
    DMA_Channel->INT1_SRCCFG = (DMA_Channel->INT1_SRCCFG
        & ~DMA_INT1_SRCCFG_INT_SRC_MASK) | 0;
    DMA_Channel->INT1_SRCCFG |= 1 << 5;
    Interrupt_enableInterrupt(DMA_INT1_IRQn);
    Interrupt_disableSleepOnIsrExit();
    
    /* configure DMA channel */
    DMA_Assign_Source_Channel(DMA_CH0_EUSCIA0TX);
    // DMA_SET_PRIO(DEBUG_UART_TX_DMA_CH);



    return ret;
}
preinit(UART_PreInit);


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
void D_Send_Nchar(char *str, uint32_t n)
{
    dma_ctrl_unit_t unit = {
        .src_addr = (str + n - 1),
        .dst_addr = &(DEBUG_UART->TXBUF),
        .ctrl = UDMA_CHCTL_SRCINC_8|UDMA_CHCTL_DSTINC_NONE|\
        UDMA_CHCTL_DSTSIZE_8|UDMA_CHCTL_SRCSIZE_8|\
        UDMA_CHCTL_ARBSIZE_1|UDMA_CHCTL_XFERMODE_BASIC|\
        ((n - 1) << 4)
    };

    /* write control word */
    DMA_Specify_Ctrl_Unit(DMA_CH0_EUSCIA0TX, &unit, CONFIG_PRIMARY);
    
    /* trigger dma transfer */
    // DMA_TRIGGER(DEBUG_UART_TX_DMA_CH);
    // DMA_GEN_SWREQ(DEBUG_UART_TX_DMA_CH);
    /* enable channel */
    DMA_Enable_Channel(DMA_CH0_EUSCIA0TX, 0);

}

/* public function */
void D_Send_String(char *str)
{
    // int i = 0;
    // for(; str[i] != '\0'; i++);
    // D_Send_Nchar(str, i);
    D_Send_Nchar(str, strlen(str));
}

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

void Printf(const char *str, ...)
{
    va_list arg_list;
    size_t i = 0;
    char *pstr = (char *)str;
    va_start(arg_list, str);
    for (; pstr[i] != '\0'; i++)
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
                    Send_String((uint8_t *)"suggest to check up the input.\r\n");
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

    return 0;
}

int D_Printf(const char *str, ...)
{
    va_list arg_list;
    va_start(arg_list, str);

    /* allocate a region for converting % */
    
    /* clear */
    memset(txbuffer, 0, MAX_TRANS_BYTES*sizeof(char));
    
    /* get final string to transmit */
    // vsnprintf(pstr, MAX_TRANS_BYTES, str, arg_list);
    vsprintf(txbuffer, str, arg_list);
    D_Send_String(txbuffer);

    va_end(arg_list);

    return 0;
}

/************* Interrupt Handler *****************/
void DMA_INT1_IRQHandler(void)
{
    // if(data_control_structure[0][3] & UDMA_CHCTL_XFERSIZE_M)
    // {
    //     DMA_TRIGGER(DEBUG_UART_TX_DMA_CH);
    // }
    // else
        // DMA_Disable_Channel(DMA_CH0_EUSCIA0TX, 0);

    Printf("INT.\r\n");
}




