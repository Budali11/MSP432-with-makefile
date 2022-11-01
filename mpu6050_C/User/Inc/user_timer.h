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
#if !defined USER_TIMER_H
#define USER_TIMER_H
#include "user_periph.h"

#define TIMER_1       0
// #define TIMER_2       1

#define TIMER_SRC_CLK             48000000

#define TIMER_1_STRUCT            TIMER32_1
#define TIMER_1_BASE              (TIMER32_BASE + 0)
#define TIMER_2_STRUCT            TIMER32_2
#define TIMER_2_BASE              (TIMER32_BASE + 0x20)

#define TIMER_1_CLK_DIVIDER         16
#define TIMER_1_CLOCK_HZ            (TIMER_SRC_CLK / TIMER_1_CLK_DIVIDER)
#define TIMER_2_CLK_DIVIDER         16
#define TIMER_2_CLOCK_HZ            (TIMER_SRC_CLK / TIMER_2_CLK_DIVIDER)


typedef struct timer
{
    Timer32_Type *base;
    float num_per_us;
    uint32_t load_value;
    void (*start)(struct timer *ptimer);
    void (*stop)(struct timer *ptimer);
    float (*read)(struct timer *ptimer);
    void (*write)(struct timer *ptimer, uint32_t load);
}timer_t;

static inline void TIMER_START(timer_t *ptimer)
{
    ptimer->base->CONTROL |= 1 << 7;
}

static inline void TIMER_STOP(timer_t *ptimer)
{
    ptimer->base->CONTROL &= ~(1 << 7);
}

static inline float TIMER_READ(timer_t *ptimer)
{
    uint32_t value = ((ptimer->base->CONTROL >> 1) & 0x1) ? (ptimer->base->VALUE) : (ptimer->base->VALUE & 0xffff);


    return (ptimer->load_value - value) / ptimer->num_per_us;
}

static inline void TIMER_WRITE(timer_t *ptimer, uint32_t load)
{
    ptimer->base->LOAD = ptimer->load_value = load;
}


#endif

