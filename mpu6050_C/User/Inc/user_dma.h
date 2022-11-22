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
#if !defined USER_DMA_H
#define USER_DMA_H

#define AVAILABLE_CHANNELS              8

//
// Channel 0
//
#define DMA_CH0_RESERVED0          0x00000000
#define DMA_CH0_EUSCIA0TX          0x01000000
#define DMA_CH0_EUSCIB0TX0         0x02000000
#define DMA_CH0_EUSCIB3TX1         0x03000000
#define DMA_CH0_EUSCIB2TX2         0x04000000
#define DMA_CH0_EUSCIB1TX3         0x05000000
#define DMA_CH0_TIMERA0CCR0        0x06000000
#define DMA_CH0_AESTRIGGER0        0x07000000

//
// Channel 1
//
#define DMA_CH1_RESERVED0          0x00000001
#define DMA_CH1_EUSCIA0RX          0x01000001
#define DMA_CH1_EUSCIB0RX0         0x02000001
#define DMA_CH1_EUSCIB3RX1         0x03000001
#define DMA_CH1_EUSCIB2RX2         0x04000001
#define DMA_CH1_EUSCIB1RX3         0x05000001
#define DMA_CH1_TIMERA0CCR2        0x06000001
#define DMA_CH1_AESTRIGGER1        0x07000001

//
// Channel 2
//
#define DMA_CH2_RESERVED0          0x00000002
#define DMA_CH2_EUSCIA1TX          0x01000002
#define DMA_CH2_EUSCIB1TX0         0x02000002
#define DMA_CH2_EUSCIB0TX1         0x03000002
#define DMA_CH2_EUSCIB3TX2         0x04000002
#define DMA_CH2_EUSCIB2TX3         0x05000002
#define DMA_CH2_TIMERA1CCR0        0x06000002
#define DMA_CH2_AESTRIGGER2        0x07000002

//
// Channel 3
//
#define DMA_CH3_RESERVED0          0x00000003
#define DMA_CH3_EUSCIA1RX          0x01000003
#define DMA_CH3_EUSCIB1RX0         0x02000003
#define DMA_CH3_EUSCIB0RX1         0x03000003
#define DMA_CH3_EUSCIB3RX2         0x04000003
#define DMA_CH3_EUSCIB2RX3         0x05000003
#define DMA_CH3_TIMERA1CCR2        0x06000003
#define DMA_CH3_RESERVED1          0x07000003

//
// Channel 4
//
#define DMA_CH4_RESERVED0          0x00000004
#define DMA_CH4_EUSCIA2TX          0x01000004
#define DMA_CH4_EUSCIB2TX0         0x02000004
#define DMA_CH4_EUSCIB1TX1         0x03000004
#define DMA_CH4_EUSCIB0TX2         0x04000004
#define DMA_CH4_EUSCIB3TX3         0x05000004
#define DMA_CH4_TIMERA2CCR0        0x06000004
#define DMA_CH4_RESERVED1          0x07000004

//
// Channel 5
//
#define DMA_CH5_RESERVED0          0x00000005
#define DMA_CH5_EUSCIA2RX          0x01000005
#define DMA_CH5_EUSCIB2RX0         0x02000005
#define DMA_CH5_EUSCIB1RX1         0x03000005
#define DMA_CH5_EUSCIB0RX2         0x04000005
#define DMA_CH5_EUSCIB3RX3         0x05000005
#define DMA_CH5_TIMERA2CCR2        0x06000005
#define DMA_CH5_RESERVED1          0x07000005

//
// Channel 6
//
#define DMA_CH6_RESERVED0          0x00000006
#define DMA_CH6_EUSCIA3TX          0x01000006
#define DMA_CH6_EUSCIB3TX0         0x02000006
#define DMA_CH6_EUSCIB2TX1         0x03000006
#define DMA_CH6_EUSCIB1TX2         0x04000006
#define DMA_CH6_EUSCIB0TX3         0x05000006
#define DMA_CH6_TIMERA3CCR0        0x06000006
#define DMA_CH6_EXTERNALPIN        0x07000006

//
// Channel 7
//
#define DMA_CH7_RESERVED0          0x00000007
#define DMA_CH7_EUSCIA3RX          0x01000007
#define DMA_CH7_EUSCIB3RX0         0x02000007
#define DMA_CH7_EUSCIB2RX1         0x03000007
#define DMA_CH7_EUSCIB1RX2         0x04000007
#define DMA_CH7_EUSCIB0RX3         0x05000007
#define DMA_CH7_TIMERA3CCR2        0x06000007
#define DMA_CH7_ADC14              0x07000007

#define DMA_TRIGGER(ch) \
    DMA_Channel->SW_CHTRIG |= (1 << ch)
#define DMA_GEN_SWREQ(ch) \
    DMA_Control->SWREQ |= (1 << ch)
#define DMA_SET_PRIO(ch) \
    DMA_Control->PRIOSET |= (1 << ch)
    
#define CONFIG_PRIMARY              1
#define CONFIG_ALTERNATE            2

typedef struct dma_ctrl_unit
{
    volatile void *src_addr;
    volatile void *dst_addr;
    volatile uint32_t ctrl;
    void *reserved;
}dma_ctrl_unit_t;

typedef struct dma_controller
{
    uint8_t cur_ch_num; /* channel C */

}dma_controller_t;


void DMA_Assign_Source_Channel(uint32_t Channel_Peripheral);
void DMA_Specify_Ctrl_Unit(uint32_t Channel_Peripheral, dma_ctrl_unit_t *unit, uint8_t PRIorALT);
void DMA_Enable_Channel(uint32_t Channel_Peripheral, uint8_t ifAlternate);
void DMA_Disable_Channel(uint32_t Channel_Peripheral, uint8_t ifAlternate);


#endif
