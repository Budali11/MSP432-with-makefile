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
#include "user_timer.h"

timer_t timer1;

static void Timer_Init(timer_t *ptimer)
{
    /* initialize ptimer */
    ptimer->start = TIMER_START;
    ptimer->stop = TIMER_STOP;
    ptimer->read = TIMER_READ;
    ptimer->write = TIMER_WRITE;

#if defined TIMER_1
    /* set up number per micro second */
    ptimer->num_per_us = TIMER_1_CLOCK_HZ / 1000000;
#endif
#if defined TIMER_2
    /* set up number per micro second */
    ptimer->num_per_us = TIMER_2_CLOCK_HZ / 1000000;
#endif
}

static int Timer_PreInit(void)
{
#if defined TIMER_1
    /* disable timer */
    TIMER_1_STRUCT->CONTROL &= ~(1 << 7);

    /* periodic mode */
    TIMER_1_STRUCT->CONTROL |= 1 << 6;

    /* disable interrupt */
    TIMER_1_STRUCT->CONTROL &= ~(1 << 5);

    /* select clock source divider(divided by 16) */
    TIMER_1_STRUCT->CONTROL |= 1 << 2;
    TIMER_1_STRUCT->CONTROL &= ~(1 << 3);

    /* select as 16-bit counter */
    TIMER_1_STRUCT->CONTROL &= ~(1 << 1);

    /* select as one shot mode */
    TIMER_1_STRUCT->CONTROL |= 1 << 0;

    /* load vlaue */
    /* set 16-bit value, save computing time */
    TIMER_1_STRUCT->LOAD = 0xffff;
#endif

#if defined TIMER_2
    /* disable timer */
    TIMER_2_STRUCT->CONTROL &= ~(1 << 7);

    /* periodic mode */
    TIMER_2_STRUCT->CONTROL |= 1 << 6;

    /* disable interrupt */
    TIMER_2_STRUCT->CONTROL &= ~(1 << 5);

    /* select clock source divider(divided by 16) */
    TIMER_2_STRUCT->CONTROL |= 1 << 2;
    TIMER_2_STRUCT->CONTROL &= ~(1 << 3);

    /* select as 16-bit counter */
    TIMER_2_STRUCT->CONTROL &= ~(1 << 1);

    /* select as one shot mode */
    TIMER_2_STRUCT->CONTROL |= 1 << 0;

    /* load vlaue */
    /* set 16-bit value, save computing time */
    TIMER_2_STRUCT->LOAD = 0xffff;
#endif

    /* initialize structures */
    Timer_Init(&timer1);

    return 0;
}
preinit(Timer_PreInit);

