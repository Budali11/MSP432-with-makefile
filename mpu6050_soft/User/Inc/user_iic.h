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

#if !defined USER_IIC_H
#define USER_IIC_H 

#include "msp.h"

/* iic rate select */
#define IIC_SUPER_SUPER_FAST_RATE   0
#define IIC_SUPER_FAST_RATE         1
#define IIC_FAST_RATE               2
#define IIC_MEDIUM_RATE             3
#define IIC_LOW_RATE                4


/* use for io base */
#define ODD 1
#define EVEN 2

/* iic operations flags */
#define IIC_STOP        0
#define IIC_NO_STOP     1
#define IIC_ACK         2
#define IIC_NACK        4


/* a enum type for iic device, indicates if this device is be used of not */
enum LOCK_T {
    LOCKED = 0,
    UNLOCK = 1
};

/* a structure type used by iic transmit or receive data */
struct iic_data
{
    uint8_t *buf;
    uint32_t num;
};

/* a structure typr represents a iic slave */
struct iic_slave
{
    LOCK_T lock;
    const char *name;
    uint16_t addr;
    struct iic_slave *next;
    void *pclass_ops;
};

class User_IIC_T
{
private:
    union iic_io_base
    {
        DIO_PORT_Even_Interruptable_Type *even_base;
        DIO_PORT_Odd_Interruptable_Type *odd_base;
    }m_base;
    uint8_t base_flag;
    struct iic_slave *m_slave_list, *m_current_slave;
    uint16_t m_sda_port, m_sda_pin, m_scl_port, m_scl_pin;
    uint32_t m_delay_value;
    inline void SDA_IN(void)
    {
        (base_flag == ODD)?(m_base.odd_base->DIR &= ~m_sda_pin, m_base.odd_base->OUT |= m_sda_pin)\
            :(m_base.even_base->DIR &= ~m_sda_pin, m_base.even_base->OUT |= m_sda_pin);
    }
    inline void SDA_OUT(void)
    {
        (base_flag == ODD)?(m_base.odd_base->DIR |= m_sda_pin)\
            :(m_base.even_base->DIR |= m_sda_pin);
    }
    inline void SDA_SET(int value)
    {
        (base_flag == ODD)?(value ? m_base.odd_base->OUT |= m_sda_pin:m_base.odd_base->OUT &= ~m_sda_pin):\
        (value ? m_base.even_base->OUT |= m_sda_pin:m_base.even_base->OUT &= ~m_sda_pin);
    }
    inline void SCL_SET(int value)
    {
        (base_flag == ODD)?(value ? m_base.odd_base->OUT |= m_scl_pin:m_base.odd_base->OUT &= ~m_scl_pin):\
        (value ? m_base.even_base->OUT |= m_scl_pin:m_base.even_base->OUT &= ~m_scl_pin);
    }
    inline bool READ_SDA(void)
    {
        return (base_flag == ODD)?(m_base.odd_base->IN & m_sda_pin):(m_base.even_base->IN & m_sda_pin);
    }
    inline void DELAY(uint32_t us)
    {
        for(uint32_t i = m_delay_value*us; i > 0; i--);
    }
    void m_iic_start(void);
    void m_iic_stop(void);
    int m_iic_send_byte(uint8_t byte);
    uint8_t m_iic_receive_byte(bool ack);
public:
    User_IIC_T(DIO_PORT_Even_Interruptable_Type *base);
    User_IIC_T(DIO_PORT_Odd_Interruptable_Type *base);
    ~User_IIC_T();
    void init(uint32_t scl_rate);
    void add_slave(iic_slave *pslave);
    uint16_t select_slave(const char *name);
    
    /**
     * @brief write data to current slave
     * @param data a pointer to a iic_data structure
     * @param flags specify if generate stop or not, it can be
     *          \b IIC_STOP
     *          \b IIC_NO_STOP
     * @note this function would always generate a start condition first
     */
    int write(struct iic_data *data, uint32_t flags);

    /**
     * @brief read data from current slave
     * @param data a pointer to a iic_data structure
     * @param flags specify if generate ack or not, it can be
     *          \b IIC_ACK
     *          \b IIC_NACK
     */
    int read(struct iic_data *data, uint32_t flags);
};

#endif
