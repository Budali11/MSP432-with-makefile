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

static dma_controller_t controller;
static uint32_t data_control_structure[][4] __attribute__((aligned(16))) = 
{
    /* first element is source end pointer */
    /* second element is destination end pointer */
    /* third element is control word */
    /* last element must remain 0 */

    /* primary data structure */
#if defined USING_1_CHANNEL
    /* channel 0 */
    {0,0,0,0},
#if defined USING_2_CHANNEL
    /* channel 1 */
    {0,0,0,0},
#if defined USING_3TO4_CHANNEL
    /* channel 2 */
    {0,0,0,0},
    /* channel 3 */
    {0,0,0,0},
#if defined USING_5TO8_CHANNEL
    /* channel 4 */
    {0,0,0,0},
    /* channel 5 */
    {0,0,0,0},
    /* channel 6 */
    {0,0,0,0},
    /* channel 7 */
    {0,0,0,0},
#endif
#endif
#endif
#endif
    /* alternate data structure */
#if defined USING_1_CHANNEL
    /* channel 0 */
    {0,0,0,0},
#if defined USING_2_CHANNEL
    /* channel 1 */
    {0,0,0,0},
#if defined USING_3TO4_CHANNEL
    /* channel 2 */
    {0,0,0,0},
    /* channel 3 */
    {0,0,0,0},
#if defined USING_5TO8_CHANNEL
    /* channel 4 */
    {0,0,0,0},
    /* channel 5 */
    {0,0,0,0},
    /* channel 6 */
    {0,0,0,0},
    /* channel 7 */
    {0,0,0,0},
#endif
#endif
#endif
#endif
};


void DMA_PreInit(void)
{
    /* enable data, user access*/
    /* non-bufferable and non-cacheable */
    DMA_Control->CFG |= 0x01 << 5;

    /*set up the base address of primary data structure */
    DMA_Control->CTLBASE |= (uint32_t)data_control_structure;

    /* enable dma controller */
    DMA_Control->CFG |= 1 << 0;
}

