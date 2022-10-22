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
#include "iic_bus.h"
int IIC_Init(iic_adapter_t *padapter)
{
    /* set up id */
    padapter->driver_id = IIC_SCL_PORT << 24 | IIC_SCL_PIN << 16 | IIC_SDA_PORT << 8 | IIC_SDA_PIN;

    /* set up functions */
    padapter->start = IIC_Adapter_Start;
    padapter->stop = IIC_Adapter_Stop;
    padapter->sendByte = IIC_Adapter_SendByte;
    padapter->receiveByte = IIC_Adapter_ReceiveByte;

    return 0;
}

int IIC_Adapter_Init(iic_adapter_t *padapter)
{
    int ret = 0;
    /* lock */
    (padapter->lock != LOCKED) ? padapter->lock = LOCKED : (ret = -1);
    if(ret != 0)
        return ret;

    /* set up gpio */
    GPIO_setAsInputPinWithPullUpResistor(IIC_GET_SDA_PORT(padapter->driver_id), IIC_GET_SDA_PIN(padapter->driver_id));
    GPIO_setAsOutputPin(IIC_GET_SCL_PORT(padapter->driver_id), IIC_GET_SCL_PIN(padapter->driver_id));
    GPIO_setOutputHighOnPin(IIC_GET_SCL_PORT(padapter->driver_id), IIC_GET_SCL_PIN(padapter->driver_id));

    /* release this driver */
    padapter->lock = UNLOCK;

    return ret;
}

int IIC_Adapter_Start(void)
{
    /* set up SDA as output */
    SDA_OUT();

    /* drive all high */
    SDA_SET(1);
    SCL_SET(1);

    /* continue 2 us*/
    DELAY(2);

    /* drive down sda when scl is high */
    SDA_SET(0);
    DELAY(2);

    /* held scl low, prepare to send message */
    SCL_SET(0);

    return 0;
}

int IIC_Adapter_Stop(void)
{
    /* set up SDA as output */
    SDA_OUT();

    /* drive scl and sda low */
    SCL_SET(0);
    DELAY(1);
    SDA_SET(0);
    /* continue 1 us*/
    DELAY(1);

    /* drive scl high */
    SCL_SET(1);
    /* continue 1 us*/
    DELAY(1);

    /* drive up sda when scl is high */
    SDA_SET(1);

    return 0;
}

int IIC_Adapter_SendByte(uint8_t byte)
{
    int ret = 0;
    /* drive SCL low */
    SCL_SET(0);
    DELAY(1);
    
    SDA_OUT();
    DELAY(1);

    /* send byte */
    for(uint8_t i = 0; i < 8; i++)
    {
        (byte & 0x80)?(SDA_SET(1)):(SDA_SET(0));
        // DELAY(1);
        
        byte <<= 1;
        SCL_SET(1);
        DELAY(1);
        SCL_SET(0);
        // DELAY(1);
    }

    /* wait ack */
    SDA_IN();
    SCL_SET(1);
    DELAY(1);
    ret = !READ_SDA();
    SCL_SET(0);
    return ret;
}

int IIC_Adapter_ReceiveByte(uint8_t ack)
{
    /* sda configure as input */
    SDA_IN();
    uint8_t receive = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        /* create a piuse */
        SCL_SET(0);
        DELAY(1);
        SCL_SET(1);

        /* pulse continue */
        DELAY(1);

        /* read sda when scl is high */
        receive <<= 1;
        if(READ_SDA())
            receive |= 1;
    }
    if(ack) /* if need a ack */
    {
        /* drive scl low*/
        SCL_SET(0);
        DELAY(1);
        
        /* drive sda low before scl high to acknowledge */
        SDA_OUT();
        SDA_SET(0);
        DELAY(1);

        /* drive up scl */
        SCL_SET(1);
        DELAY(2);
        
        /* drive scl low*/
        SCL_SET(0);
        DELAY(1);
    }
    else /* if not need a acknowledge */
    {
        /* drive scl low*/
        SCL_SET(0);
        DELAY(1);
        
        /* sda as input before scl high to nacknowledge */
        SDA_IN();
        DELAY(1);

        /* drive up scl */
        SCL_SET(1);
        DELAY(2);
    }

    return receive;
}



