#include "msp.h"
#include "gpio.h"

/**
 * main.c
 */
int main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
	while(1)
	{
		
	}
	return 0;
}
