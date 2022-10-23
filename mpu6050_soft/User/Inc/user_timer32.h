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

#if !defined USER_TIMER32_H
#define USER_TIMER32_H

#include "msp.h"

#define TIMER_1       0
#define TIMER_2       1

class User_Timer32_T
{
private:
    Timer32_Type *m_base;
public:
    User_Timer32_T(/* args */);
    ~User_Timer32_T();
    void init(Timer32_Type *base, uint8_t mode);
    inline void start(void){
        m_base->CONTROL |= 1 << 7;
    }
    inline void restart(void){
        m_base->LOAD = 0xffff;
    }
    inline void stop(void){
        m_base->CONTROL &= ~(1 << 7);
    }
    inline void int_enable(void){
        m_base->CONTROL |= 1 << 5;
    }
    inline uint32_t read_timer(void){
        return ((m_base->CONTROL >> 1) & 0x1) ? (m_base->VALUE) : (m_base->VALUE & 0xffff);
    }
    inline void load(uint32_t load){
        m_base->LOAD = load;
    }

};

class clock : public User_Timer32_T
{
private:
    uint32_t m_load;
    float m_us;
public:
    clock();
    clock(uint8_t timer_n);
    ~clock();
    void clock_init(void);
    void operator=(uint32_t us);
    void operator>>(float & rtime);
};


#endif 
