#include "user_core.h"
#include "gpio.h"
#include "cs.h"
#include "user_uart.h"
#include "user_timer32.h"
#include "flash.h"
#include "pcm.h"

void Clock_Configuration(void);
void Clock_Information(void);
User_Uart_T yuki = User_Uart_T(EUSCI_A0, 115200);
extern User_Systick_T user_systick;
extern clock user_clock1;

/**
 * main.c
 */
int main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
	Clock_Configuration();
    yuki.init(EUSCI_A0, 115200);
    yuki.send_string("\r\nYuki.\r\n");
    Clock_Information();

    // iic.init(IIC_SUPER_SUPER_FAST_RATE);
    // iic.add_slave(mpu6050.iic_init());
    // iic.select_slave("mpu6050");
    
    // if(mpu6050.device_init())
    //     yuki.printf("device mpu6050 init failed.\r\n");
    // else
    //     yuki.printf("\r\ndevice mpu6050 init ok.\r\n");

	while(1)
	{
        // mpu6050.read_all(MPU6050_DEBUG);
        // mpu6050.kalman_getAngle(MPU6050_DEBUG);
	}
	return 0;
}

void Clock_Configuration(void)
{
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000, 48000000);

    PCM_setCoreVoltageLevel(PCM_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false);
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1); //ACLK == 32.768kHz
    CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1); //MCLK == 48MHz
    CS_initClockSignal(CS_HSMCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_1); //HSMCLK == 24MHz
    CS_initClockSignal(CS_SMCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_2); //SMCLK == 12MHz
    CS_initClockSignal(CS_BCLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1); //BCLK == 32.768MHz

}

void Clock_Information(void)
{
    uint32_t ACLK, MCLK, HSMCLK, SMCLK, BCLK;

    ACLK = CS_getACLK();
    MCLK = CS_getMCLK();
    HSMCLK = CS_getHSMCLK();
    SMCLK = CS_getSMCLK();
    BCLK = CS_getBCLK();
    yuki.printf("ACLK: %u\t MCLK:%u\r\nHSMCLK: %u\t SMCLK: %u\r\n BCLK: %u\r\n", ACLK, MCLK, HSMCLK, SMCLK, BCLK);
}