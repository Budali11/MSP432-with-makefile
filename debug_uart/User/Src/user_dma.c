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
#include "msp.h"
#include "user_dma.h"
#include "user_periph.h"
#include "user_uart.h"

dma_ctrl_unit_t data_control_structure[] __attribute__((aligned(256))) = 
{
    /* first element is source end pointer */
    /* second element is destination end pointer */
    /* third element is control word */
    /* last element must remain 0 */

    /* primary data structure */
    /* channel 0 */
    {0,0,0,0},
    /* channel 1 */
    {0,0,0,0},
    /* channel 2 */
    {0,0,0,0},
    /* channel 3 */
    {0,0,0,0},
    /* channel 4 */
    {0,0,0,0},
    /* channel 5 */
    {0,0,0,0},
    /* channel 6 */
    {0,0,0,0},
    /* channel 7 */
    {0,0,0,0},
    /* alternate data structure */
    /* channel 0 */
    {0,0,0,0},
    /* channel 1 */
    {0,0,0,0},
    /* channel 2 */
    {0,0,0,0},
    /* channel 3 */
    {0,0,0,0},
    /* channel 4 */
    {0,0,0,0},
    /* channel 5 */
    {0,0,0,0},
    /* channel 6 */
    {0,0,0,0},
    /* channel 7 */
    {0,0,0,0},
};

static int DMA_PreInit(void)
{
    /* enable dma controller */
    DMA_Control->CFG = 1 << 0;

    /* set up the base address of primary data structure */
    DMA_Control->CTLBASE = (uint32_t)data_control_structure;

    // /* clear attributes bits */
    // DMA_Control->ALTCLR = 0xffffffff;
    // DMA_Control->USEBURSTCLR = 0xffffffff;
    // DMA_Control->PRIOCLR = 0xffffffff;
    // DMA_Control->REQMASKCLR = 0xffffffff;
    
    return 0;
}
preinit(DMA_PreInit);

void DMA_Assign_Source_Channel(uint32_t Channel_Peripheral)
{
    /* configure SRCCFG register */
    DMA_Channel->CH_SRCCFG[Channel_Peripheral & 0x1] = Channel_Peripheral >> 24;
}

void DMA_Specify_Ctrl_Unit(uint32_t Channel_Peripheral, dma_ctrl_unit_t *unit, uint8_t PRIorALT)
{
    /* configure data control structure */
    if(PRIorALT & CONFIG_PRIMARY)
    {
        data_control_structure[Channel_Peripheral & 0x1].src_addr = unit->src_addr;
        data_control_structure[Channel_Peripheral & 0x1].dst_addr = unit->dst_addr;
        data_control_structure[Channel_Peripheral & 0x1].ctrl = unit->ctrl;
    }
    if(PRIorALT & CONFIG_ALTERNATE)
    {
        data_control_structure[(Channel_Peripheral & 0x1) + 8].src_addr = unit->src_addr;
        data_control_structure[(Channel_Peripheral & 0x1) + 8].dst_addr = unit->dst_addr;
        data_control_structure[(Channel_Peripheral & 0x1) + 8].ctrl = unit->ctrl;
    }
}

void DMA_Enable_Channel(uint32_t Channel_Peripheral, uint8_t ifAlternate)
{
    ifAlternate ? (DMA_Control->ALTSET = (1 << (Channel_Peripheral & 0x1))) : \
        (DMA_Control->ALTCLR = (1 << (Channel_Peripheral & 0x1)));
        
    DMA_Control->ENASET = (1 << (Channel_Peripheral & 0x1));
}

void DMA_Disable_Channel(uint32_t Channel_Peripheral, uint8_t ifAlternate)
{
    (ifAlternate) ? (DMA_Control->ALTCLR = (1 << (Channel_Peripheral & 0xf))) : \
        (DMA_Control->ALTSET = (1 << (Channel_Peripheral & 0xf)));
        
    DMA_Control->ENACLR = (1 << (Channel_Peripheral & 0xf));
}

/************* Interrupt Handler *****************/
void DMA_ERR_IRQHandler(void)
{
    // if(data_control_structure[0][3] & UDMA_CHCTL_XFERSIZE_M)
    // {
    //     DMA_TRIGGER(DEBUG_UART_TX_DMA_CH);
    // }
    // else
        // DMA_Disable_Channel(DMA_CH0_EUSCIA0TX, 0);

    Printf("INT.\r\n");
}
