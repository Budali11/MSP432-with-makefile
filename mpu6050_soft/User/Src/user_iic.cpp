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

#include "user_iic.h"
#include "cs.h"
#include "gpio.h"
#include "cstring"


void User_IIC_T::m_iic_start(void)
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
}
void User_IIC_T::m_iic_stop(void)
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
}


int User_IIC_T::m_iic_send_byte(uint8_t byte)
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

uint8_t User_IIC_T::m_iic_receive_byte(bool ack)
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

User_IIC_T::User_IIC_T(DIO_PORT_Even_Interruptable_Type *base)
{
    m_base.even_base = base;
    m_current_slave = nullptr;
    m_current_slave = nullptr;
    m_scl_port = GPIO_PORT_P6;
    m_scl_pin = GPIO_PIN5;
    m_sda_port = GPIO_PORT_P6;
    m_sda_pin = GPIO_PIN4;
    base_flag = EVEN;
}
User_IIC_T::User_IIC_T(DIO_PORT_Odd_Interruptable_Type *base)
{
    m_base.odd_base = base;
    m_current_slave = nullptr;
    m_current_slave = nullptr;
    m_scl_port = GPIO_PORT_P6;
    m_scl_pin = GPIO_PIN5;
    m_sda_port = GPIO_PORT_P6;
    m_sda_pin = GPIO_PIN4;
    base_flag = ODD;
}


User_IIC_T::~User_IIC_T()
{
}

/**
 * @brief initialize iic adapter
 */
void User_IIC_T::init(uint32_t scl_rate)
{
    /* configure ports */
    GPIO_setAsOutputPin(m_scl_port, m_scl_pin);
    GPIO_setOutputHighOnPin(m_scl_port, m_scl_pin);
    GPIO_setAsInputPinWithPullUpResistor(m_sda_port, m_sda_pin);

    if(scl_rate == IIC_SUPER_SUPER_FAST_RATE)
        m_delay_value = 1; 
    else if(scl_rate == IIC_SUPER_FAST_RATE)
        m_delay_value = 3; 
    else if(scl_rate == IIC_FAST_RATE)
        m_delay_value = 6;
    else if(scl_rate == IIC_MEDIUM_RATE)
        m_delay_value = 12;
    else if(scl_rate == IIC_LOW_RATE)
        m_delay_value = 24;
}

void User_IIC_T::add_slave(iic_slave *pslave)
{
    if(m_slave_list == nullptr)
    {
        m_slave_list = pslave;
    }
    else
    {
        iic_slave *tmp = m_slave_list;
        for(;tmp->next != nullptr;tmp = tmp->next);
        if(tmp->next == nullptr)
            tmp->next = pslave;
    }
    pslave->pclass_ops = (void *)this;
}
/**
 * @brief find a slave through name
 * @param name the specific name
 * @retval the slave address if the specific slave is found 
 *          or 0 if is not found
 */
uint16_t User_IIC_T::select_slave(const char *name)
{
    struct iic_slave *ps = m_slave_list;
    /* find slave through name */
    for(;(ps->next != 0) && (strcmp(ps->name, name) != 0); ps = ps->next);

    /* if found it */
    if(strcmp(ps->name, name) == 0)
    {
        m_current_slave = ps;
        m_current_slave->lock = LOCKED;

        return ps->addr;
    }
    else
    {
        /* print error */
        return 0;
    }
}

/**
 * @brief write data to current slave
 * @param data a pointer to a iic_data structure
 * @param flags specify if generate stop or not, it can be
 *          \b IIC_STOP
 *          \b IIC_NO_STOP
 * @retval ack times
 */
int User_IIC_T::write(struct iic_data *data, uint32_t flags)
{
    int ret = 0;
    /* generate start condition */
    m_iic_start();
    /* transmit process */
    for(uint32_t i = 0; i < data->num; i++)
    {
        /* send bytes */
        ret += m_iic_send_byte(data->buf[i]);
    }
    DELAY(1);

    if(flags & IIC_NO_STOP != 0) /* if not need to stop */
        return ret;

    /* generate stop as default*/
    m_iic_stop();
    
    return ret;
}

/**
 * @brief read data from current slave
 */
int User_IIC_T::read(struct iic_data *data, uint32_t flags)
{
    /* receive data process */
    for(uint32_t i = 0; i < data->num; i++)
    {
        /* receive bytes */
        data->buf[i] = m_iic_receive_byte((i != (data->num - 1)) && (IIC_ACK & flags));
    }
    
    if(flags & IIC_NO_STOP != 0) /* if not need to stop */
        return 0;

    /* generate stop as default*/
    m_iic_stop();

    return 0;
}
