#include "user_core.h"
#include "gpio.h"
#include "cs.h"
#include "user_uart.h"

void Clock_Configuration(void);
User_Uart_T yuki = User_Uart_T(EUSCI_A0, 115200);
extern User_Systick_T user_systick;

/**
 * main.c
 */
int main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	Clock_Configuration();
    //User_All_Device_Init();
    yuki.init(EUSCI_A0, 115200);
    yuki.send_string("Yuki.\r\n");
    user_systick.init();
    user_systick.set_period(1000);
    user_systick.set_interrupt(1, true);
    user_systick.start();

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
