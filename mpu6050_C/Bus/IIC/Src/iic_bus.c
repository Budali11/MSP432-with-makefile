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
#include "user_periph.h"

iic_adapter_t user_iic;

static int IIC_Adapter_Init(iic_adapter_t *padapter)
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

static int IIC_Init(iic_adapter_t *padapter)
{
    int ret = 0;
    /* set up id */
    padapter->driver_id = IIC_SCL_PORT << 24 | IIC_SCL_PIN << 16 | IIC_SDA_PORT << 8 | IIC_SDA_PIN;

    /* set up functions */
    padapter->init = IIC_Adapter_Init;
    padapter->read = IIC_Adapter_Read;
    padapter->write = IIC_Adapter_Write;

    /* adapter lock unlock as default */
    padapter->lock = UNLOCK;

    /* call adapter init function */
    ret = padapter->init(padapter);

    return ret;
}

static int IIC_preInit(void)
{
    int ret = 0;
    ret = IIC_Init(&user_iic);

    return ret;
}
preinit(IIC_preInit);

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

/**
 * @brief write datas to soft iic bus
 * @param padapter pointer points to the iic adapter, it points defaultly to user_iic
 * @param buf pointer points to first data
 * @param num specify how many bytes to be transfered
 * @param flags use this to specify some special options
 *          - \b IIC_STOP        use this flag will generate a stop signal in the end of this function
 *          - \b IIC_NO_STOP     on the contrary of \b IIC_STOP
 *          - \b IIC_ACK         use this flag will generate a acknowledge signal in the end of this function, but before stop
 *          - \b IIC_NACK        on the contrary of \b IIC_ACK, same before stop
 * @retval 0 if success, -1 if iic adapter is busy
 */
int IIC_Adapter_Write(iic_adapter_t *padapter, uint8_t *buf, uint32_t num, uint32_t flags)
{
    int ret = 0;

    /* lock this driver */
    if(padapter->lock != UNLOCK)
    {
        //perror
        return -1;
    }
    padapter->lock = LOCKED;

    /* generate start condition */
    IIC_Adapter_Start();

    /* transmit process */
    for(uint32_t i = 0; i < num; i++)
    {
        /* send bytes */
        ret += IIC_Adapter_SendByte(buf[i]);
    }
    DELAY(1);

    if((flags & IIC_NO_STOP) != 0) /* if not need to stop */
    {
        /* release this driver */
        padapter->lock = UNLOCK;
        return ret;
    }

    /* generate stop as default*/
    IIC_Adapter_Stop();

    /* release this driver */
    padapter->lock = UNLOCK;
    
    return ret;
}

/**
 * @brief receive datas to soft iic bus
 * @param padapter pointer points to the iic adapter, it points defaultly to user_iic
 * @param buf pointer points to where you store data
 * @param num specify how many bytes to be received
 * @param flags use this to specify some special options
 *          - \b IIC_STOP        use this flag will generate a stop signal in the end of this function
 *          - \b IIC_NO_STOP     on the contrary of \b IIC_STOP
 *          - \b IIC_ACK         use this flag will generate a acknowledge signal in the end of this function, but before stop
 *          - \b IIC_NACK        on the contrary of \b IIC_ACK, same before stop
 * @retval 0 if success, -1 if iic adapter is busy
 */
int IIC_Adapter_Read(iic_adapter_t *padapter, uint8_t *buf, uint32_t num, uint32_t flags)
{
    /* lock this driver */
    if(padapter->lock != UNLOCK)
    {
        //perror
        return -1;
    }
    padapter->lock = LOCKED;

    /* receive data process */
    for(uint32_t i = 0; i < num; i++)
    {
        /* receive bytes */
        buf[i] = IIC_Adapter_ReceiveByte((i != (num - 1)) && (IIC_ACK & flags));
    }
    
    if((flags & IIC_NO_STOP) != 0) /* if not need to stop */
    {
        /* release this driver */
        padapter->lock = UNLOCK;
        return 0;
    }
    /* generate stop as default*/
    IIC_Adapter_Stop();

    /* release this driver */
    padapter->lock = UNLOCK;

    return 0;
}


