/*
 * user_uart.h
 *
 *  Created on: 2022��7��27��
 *      Author: 17yearoldHaoNan
 */

#ifndef USER_INC_USER_UART_H_
#define USER_INC_USER_UART_H_

#include "msp.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
/*user can change this macro to specify a EUSCI module*/
#define USING_EUSCI_A_X 0

//ports definition
#if (USING_EUSCI_A_X == 0)
    #define UART_TXD_PIN GPIO_PIN3
    #define UART_TXD_PORT GPIO_PORT_P1
    #define UART_RXD_PIN GPIO_PIN2
    #define UART_RXD_PORT GPIO_PORT_P1
#elif (USING_EUSCI_A_X == 1)
    #define UART_TXD_PIN GPIO_PIN3
    #define UART_TXD_PORT GPIO_PORT_P1
    #define UART_RXD_PIN GPIO_PIN2
    #define UART_RXD_PORT GPIO_PORT_P1
#elif (USING_EUSCI_A_X == 2)
    #define UART_TXD_PIN GPIO_PIN3
    #define UART_TXD_PORT GPIO_PORT_P1
    #define UART_RXD_PIN GPIO_PIN2
    #define UART_RXD_PORT GPIO_PORT_P1
#elif (USING_EUSCI_A_X == 3)
    #define UART_TXD_PIN GPIO_PIN3
    #define UART_TXD_PORT GPIO_PORT_P1
    #define UART_RXD_PIN GPIO_PIN2
    #define UART_RXD_PORT GPIO_PORT_P1
#elif (USING_EUSCI_A_X == 4)
    #define UART_TXD_PIN GPIO_PIN3
    #define UART_TXD_PORT GPIO_PORT_P1
    #define UART_RXD_PIN GPIO_PIN2
    #define UART_RXD_PORT GPIO_PORT_P1
#else
    #error please specify a EUSCI module

#endif 

//baudrate definitions, write to MCTLW register
#define BAUDRATE_9600       0x21
#define BAUDRATE_19200      0x11
#define BAUDRATE_38400      0x6581
#define BAUDRATE_57600      0x2501
#define BAUDRATE_115200     0x2081
#define BAUDRATE_230400     0x2041
#define BAUDRATE_460800     0xA1

class User_Uart_T
{
private:
    EUSCI_A_Type *m_base;
    uint32_t m_baudrate;
    void m_send_Nchar(uint8_t *str, uint32_t n);
public:
    User_Uart_T(EUSCI_A_Type *base, uint32_t baudrate);
    ~User_Uart_T();
    void init(EUSCI_A_Type *base, uint32_t baudrate);
    void send_string(uint8_t *str);
    void send_string(const char *str);
    uint32_t receive_string(uint8_t *tmp);
    void printf(const char *str, ...);
};





#endif /* USER_INC_USER_UART_H_ */
