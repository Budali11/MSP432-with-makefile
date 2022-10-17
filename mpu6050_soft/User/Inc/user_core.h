/**
 * Copyleft ! 2022/8/6  by Budali11  
 * @brief   This file may content sonme functions about the cortex, 
 *          such as nvic, watch dog
 */
#include "msp.h"
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#define Enable 0
#define Disable 1

typedef struct __list_node
{
    struct __list_node * next;
    int16_t IRQn_num;
}list_node;

//systick class definition
class User_Systick_T
{
private:
    SysTick_Type *m_base;
    uint32_t m_period_ms; //user-need period 
    uint16_t m_times; //if reload-value is too big for reload register,
                    // then divide reload-value user.times times to load in the reload register
    uint16_t m_tmp; //decrement value
public:
    User_Systick_T();
    ~User_Systick_T();
    void set_period(uint32_t ms);
    void start(void);
    void stop(void);
    void init(void);
    void set_interrupt(uint32_t priority, bool status);
    void handler(void);
    void timing(uint32_t ms);
};

//NVIC class definition
class User_Nvic_T
{
private:
    list_node *enable_irq_list, *current_node;
    uint32_t priority_group;
public:
    void grouping(uint32_t PriorityGroup);
    void set_up_IRQn(IRQn_Type IRQn, uint16_t priority, bool status);
    uint8_t show_enable_irqs(void);
};
