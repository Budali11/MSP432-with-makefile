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

#if !defined USER_UART_H
#define USER_UART_H

#include "msp.h"
#include "gpio.h"
#include <stdarg.h>
#include <stdio.h>

/* maximum bytes transmits by D_Printf */
#define MAX_TRANS_BYTES 128

/*user can change this macro to specify a EUSCI module*/
#define USING_EUSCI_A_X 0

/*user can change this macro to specify a EUSCI module*/
#define DEBUG_UART              EUSCI_A0
#define DEBUG_UART_BAUDRATE     115200
#define DEBUG_UART_TX_DMA_CH    0

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


/* private function */
void D_Send_Nchar(char *str, uint32_t n);
void Send_Nchar(uint8_t *str, uint32_t n);

/* public function */
void D_Send_String(char *str);
void Send_String(uint8_t *str);
void Printf(const char *str, ...);
int Receive(uint8_t *buf);
int D_Printf(const char *str, ...);
int D_Receive(uint8_t *buf);

#endif
