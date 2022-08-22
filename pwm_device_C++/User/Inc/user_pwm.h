/**
 * made by Budali11
 * Copyleft Budali11 2022/8/3
 */
#ifndef USER_UART_H
#define USER_UART_H

//user define
#define TIMER_UNIT_1    0x02
#define TIMER_UNIT_2    0x04
#define TIMER_UNIT_3    0x08
#define TIMER_UNIT_4    0x10

#include "msp.h"

class User_PWM_T
{
private:
    Timer_A_Type *m_base; //configure registers through this pointer
    uint8_t m_pin; //the output pin
    uint8_t m_unit[6]; //unit enable array. for example, if m_unit[0] == 1, then Timer_Ax.0 is enabled
public:
    User_PWM_T(Timer_A_Type *base);
    ~User_PWM_T();
    void init(uint8_t unit_n);
    void generate();
    void setPWM(uint32_t nHz, uint32_t percent, uint8_t unit_n);
};

#endif // !USER_UART_H


