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

#include "msp.h"
#include "user_uart.h"
#include "user_pwm.h"
#include "cs.h"

void Clock_Configuration(void);
User_Uart_T yuki = User_Uart_T(EUSCI_A0, 115200);
User_PWM_T led = User_PWM_T(TIMER_A0);



/**
 * main.c
 */
int main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	Clock_Configuration();
	yuki.init(EUSCI_A0, 115200);
    led.init(TIMER_UNIT_1|TIMER_UNIT_2|TIMER_UNIT_3|TIMER_UNIT_4);
    led.setPWM(400, 20, TIMER_UNIT_1);
    led.setPWM(400, 40, TIMER_UNIT_2);
    led.setPWM(400, 60, TIMER_UNIT_3);
    led.setPWM(400, 80, TIMER_UNIT_4);
    led.generate();

	while(1)
	{
		
	}
	return 0;
}

void Clock_Configuration(void)
{
    uint32_t ACLK, MCLK, HSMCLK, SMCLK, BCLK;
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1); //ACLK == 32.768kHz
    CS_initClockSignal(CS_MCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_1); //MCLK == 24MHz
    CS_initClockSignal(CS_HSMCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_1); //HSMCLK == 24MHz
    CS_initClockSignal(CS_SMCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_2); //SMCLK == 12MHz
    CS_initClockSignal(CS_BCLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);//BCLK == 32.768MHz

    ACLK = CS_getACLK();
    MCLK = CS_getMCLK();
    HSMCLK = CS_getHSMCLK();
    SMCLK = CS_getSMCLK();
    BCLK = CS_getBCLK();
}
