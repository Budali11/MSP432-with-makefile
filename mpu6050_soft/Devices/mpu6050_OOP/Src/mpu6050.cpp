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
#include "mpu6050.h"
#include "mpu6050_defs.h"
#include "stdio.h"
#include "cmath"
#include "user_iic.h"
#include "user_uart.h"

extern User_Uart_T yuki;
clock user_clock1 = clock(TIMER_1);

MPU6050_T::MPU6050_T()
{
    m_slave_mpu6050 = (iic_slave *)malloc(sizeof(iic_slave));
    m_slave_mpu6050->name = "mpu6050";
    m_slave_mpu6050->addr = MPU6050_ADDR;

    /* measurement configuration */
    m_accel_fsr = ACCEL_4G_RANGE;
    m_gyro_fsr = GYRO_250D_RANGE;

    m_data = {
            0,
            .k = {
            .r_K = 0.4,
            .p_K = 0.4,
            .re_est = 0.0025,
            .pe_est = 0.0025,
        },
    };
}

MPU6050_T::~MPU6050_T()
{

}

iic_slave * MPU6050_T::iic_init(void)
{
    if(m_slave_mpu6050 != nullptr)
        return m_slave_mpu6050;
    return nullptr;
}

int MPU6050_T::device_init(void)
{   
    int ret = 0;
    uint8_t msg[3] = {0};
    struct iic_data data = {
        .buf = msg,
        .num = 2,
    };

    /* get operation functions */
    User_IIC_T *ops = (User_IIC_T *)m_slave_mpu6050->pclass_ops;
    

    /* check out device id */

    /* send slave address(write) and mpu6050 id register address */
    msg[0] = ((m_slave_mpu6050->addr << 1)|0);
    msg[1] = WHO_AM_I_REG;
    ret = ops->write(&data, IIC_NO_STOP);
    if(ret == 0)
    {
        //perror
        yuki.send_string("write mpu6050 nack error.\r\n");
        return -1;
    }
    
    /* send slave address(read) */
    msg[0] = ((m_slave_mpu6050->addr << 1)|1);
    data.num = 1;
    ret = ops->write(&data, IIC_NO_STOP);
    if(ret == 0)
    {
        //perror
        yuki.send_string("read mpu6050 nack error.\r\n");
        return -1;
    }

    data.buf = (uint8_t *)(msg + 2);
    ops->read(&data, IIC_NACK|IIC_STOP);
    if(msg[2] != 104)
    {
        //perror
        char debug[10] = {0};
        sprintf(debug, "reg is %d\r\n", msg[2]);
        yuki.send_string("read mpu6050 who_am_i reg error.\r\n");
        yuki.send_string(debug);
        return -1;
    }

    /* set up power management register */
    /* mpu6050 slave address(write) */
    msg[0] = ((m_slave_mpu6050->addr << 1)|0);
    msg[1] = PWR_MGMT_1_REG; /* desire register to write */
    msg[2] = 0x00; /* data write into desire register */
    data.buf = msg;
    data.num = 3;
    ret = ops->write(&data, IIC_STOP);
    if(ret != 3)
    {
        //perror
        yuki.send_string("wake up mpu6050 failed.\r\n");
        return -1;
    }

    /* set up sample rate */
    msg[1] = SMPLRT_DIV_REG;
    msg[2] = 0x07; /* sample rate 1kHz without DLPF */
    //msg[2] = 0x00; /* sample rate 1kHz but need to enable DLPF */
    ret = ops->write(&data, IIC_STOP);
    if(ret != 3)
    {
        //perror
        yuki.send_string("set up sample rate failed.\r\n");
        return -1;
    }

    /* configure accelerometer configration register */
    /* set full scale range */
    msg[1] = ACCEL_CONFIG_REG;
    msg[2] = m_accel_fsr << 3;
    ret = ops->write(&data, IIC_STOP);
    if(ret != 3)
    {
        //perror
        yuki.printf("set up accel configuration failed.\r\n");
        return -1;
    }

    /* configure gyroscope configration register */
    /* set full scale range */
    msg[1] = GYRO_CONFIG_REG;
    msg[2] = m_gyro_fsr << 3;
    ret = ops->write(&data, IIC_STOP);
    if(ret != 3)
    {
        //perror
        yuki.printf("set up gyroscope configuration failed.\r\n");
        return -1;
    }

    /* give up first 300 data */
    for(int i = 0; i < 300; i++)
        read_all(MPU6050_NO_COMPUTE);

    /* make statistics and average */
    for(uint8_t i = 0; i < 50; i++)
    {
        read_all(MPU6050_NO_DEBUG);
        /* statistics */
        m_data.ax_raw_offset += m_data.Accel_X_RAW;
        m_data.ay_raw_offset += m_data.Accel_Y_RAW;
        m_data.az_raw_offset += (int)accel_LSB[m_accel_fsr] - m_data.Accel_Z_RAW;
        m_data.gx_raw_offset += m_data.Gyro_X_RAW;
        m_data.gy_raw_offset += m_data.Gyro_Y_RAW;
        m_data.gz_raw_offset += m_data.Gyro_Z_RAW;
    }
    /* average */
    m_data.ax_raw_offset /= 50;
    m_data.ay_raw_offset /= 50;
    m_data.az_raw_offset /= 50;
    m_data.gx_raw_offset /= 50;
    m_data.gy_raw_offset /= 50;
    m_data.gz_raw_offset /= 50;

    yuki.printf("ax_offset: %d\t", m_data.ax_raw_offset);
    yuki.printf("ay_offset: %d\t", m_data.ay_raw_offset);
    yuki.printf("az_offset: %d\r\n", m_data.az_raw_offset);

    user_clock1.clock_init();

    return 0;
}
int MPU6050_T::read_all(uint8_t flags)
{
    uint8_t measure[14] = {0};
    uint32_t ret = 0;
    /* specify the first register to be read */
    uint8_t msg[2] = {0};
    msg[0] = ((m_slave_mpu6050->addr << 1)|0);
    msg[1] = ACCEL_XOUT_H_REG;
    iic_data data = {
        .buf = msg,
        .num = 2,
    };

    User_IIC_T *ops = (User_IIC_T *)m_slave_mpu6050->pclass_ops;
    ret = ops->write(&data, IIC_NO_STOP);
    if(ret != 2)
    {
        //perror
        yuki.printf("specify register, read_all error.\r\n");
        return -1;
    }

    /* slave address(read) */
    msg[0] = (m_slave_mpu6050->addr << 1)|1;
    data.num = 1;
    ret = ops->write(&data, IIC_NO_STOP);
    if(ret != 1)
    {
        //perror
        yuki.printf("reading register, read_all error.\r\n");
        return -1;
    }

    /* read out last time timer value */
    user_clock1 >> m_data.pass_us;

    /* timing */
    user_clock1 = 0;

    /* read out data */
    data.buf = measure;
    data.num = 14;
    ops->read(&data, IIC_STOP|IIC_ACK);

    if((flags & MPU6050_NO_COMPUTE) != 0)
        return 0;

    m_data.Accel_X_RAW = measure[0] << 8 | measure[1];
    m_data.Accel_Y_RAW = measure[2] << 8 | measure[3];
    m_data.Accel_Z_RAW = measure[4] << 8 | measure[5];
    int16_t tmp = measure[6] << 8 | measure[7];
    m_data.Gyro_X_RAW = measure[8] << 8 | measure[9];
    m_data.Gyro_Y_RAW = measure[10] << 8 | measure[11];
    m_data.Gyro_Z_RAW = measure[12] << 8 | measure[13];

    /* computing accel, gyro and temperature */
    m_data.Ax = (double)(m_data.Accel_X_RAW + m_data.ax_raw_offset) / accel_LSB[m_accel_fsr];
    m_data.Ay = (double)(m_data.Accel_Y_RAW + m_data.ay_raw_offset) / accel_LSB[m_accel_fsr];
    m_data.Az = (double)(m_data.Accel_Z_RAW + m_data.az_raw_offset) / accel_LSB[m_accel_fsr];

    m_data.Temperature = tmp/340.0f + 36.53f;

    m_data.Gx = (double)(m_data.Gyro_X_RAW + m_data.gx_raw_offset) / gyro_LSB[m_gyro_fsr];
    m_data.Gy = (double)(m_data.Gyro_Y_RAW + m_data.gy_raw_offset) / gyro_LSB[m_gyro_fsr];
    m_data.Gz = (double)(m_data.Gyro_Z_RAW + m_data.gz_raw_offset) / gyro_LSB[m_gyro_fsr];
    
    if((flags & MPU6050_NO_DEBUG) != 0)
        return 0;
    
    // /* output accel x */
    // yuki.printf("Ax: %f\t", m_data.Ax);
    // /* output accel y */
    // yuki.printf("Ay: %f\t", m_data.Ay);
    // /* output accel z */
    // yuki.printf("Az: %f\t", m_data.Az);
    // /* output temp */
    // yuki.printf("temp %f\t", m_data.Temperature);
    // /* output gyro x */
    // yuki.printf("Gx: %f\t", m_data.Gx);
    // /* output gyro y */
    // yuki.printf("Gy: %f\t", m_data.Gy);
    // /* output gyro z */
    // yuki.printf("Gz: %f\t", m_data.Gz);
    /* output delta t */
    yuki.printf("dt:%fus  ",m_data.pass_us);


    return 0;
}
    
void MPU6050_T::kalman_getAngle(uint8_t flags)
{
    float a_roll, a_pitch;
    static float g_roll = 0, g_pitch = 0, g_yaw = 0, e_mea = 0.5;
    double dr_dt, dp_dt, dy_dt;

    /* get a_roll and a_pitch according to accel data */
    a_roll = atan(m_data.Ay / m_data.Az) * RAD2DEG;
    a_pitch = -atan(m_data.Ax / (pow(m_data.Ay * m_data.Ay + m_data.Az * m_data.Az, 0.5))) * RAD2DEG;

    /* get g_roll, g_pitch and g_yaw according to gyro data */
    double g_rotate[][3] = 
    {   
        /**
         *  first:
         *          1, 0, 0
         *          0, 1, 0
         *          0, 0, 1 
         */
        // {1, sin(g_pitch)*sin(g_roll)/cos(g_pitch), cos(g_roll)*sin(g_pitch)/cos(g_pitch)},
        // {0, cos(g_roll), -sin(g_roll)},
        // {0, sin(g_roll)/cos(g_pitch), cos(g_roll)/cos(g_pitch)},
        {1, tan(g_pitch)*sin(g_roll), cos(g_roll)*tan(g_pitch)},
        {0, cos(g_roll), -sin(g_roll)},
        {0, sin(g_roll)/cos(g_pitch), cos(g_roll)/cos(g_pitch)}
    };

    /* compute rotated angular_v */
    dr_dt = m_data.Gx + g_rotate[0][1] * m_data.Gy + g_rotate[0][2] * m_data.Gz;
    dp_dt = g_rotate[1][1] * m_data.Gy + g_rotate[1][2] * m_data.Gz;
    dy_dt = g_rotate[2][1] * m_data.Gy + g_rotate[2][2] * m_data.Gz;

    /* g_roll and others are angle after rotated*/
    g_roll += dr_dt * m_data.pass_us / 1000000.0f;
    g_pitch += dp_dt * m_data.pass_us / 1000000.0f;
    g_yaw += dy_dt * m_data.pass_us / 1000000.0f;
    
    // /* update kalman parameter */
    // m_data.k.r_K = m_data.k.re_est / (m_data.k.re_est - e_mea);
    // m_data.k.p_K = m_data.k.pe_est / (m_data.k.pe_est - e_mea);

    // /* compute k_roll and k_pitch */
    // m_data.k.roll = a_roll + m_data.k.r_K * (a_roll - g_roll);
    // m_data.k.pitch = a_pitch + m_data.k.p_K * (a_pitch - g_pitch);
    // m_data.k.yaw = g_yaw;

    // /* update kalman parameter */
    // m_data.k.re_est = (1 - m_data.k.r_K) * m_data.k.re_est;
    // m_data.k.pe_est = (1 - m_data.k.p_K) * m_data.k.pe_est;

    if((flags & MPU6050_NO_DEBUG) != 0)
        return;
    
    yuki.printf("ar:%f  ", a_roll);
    // yuki.printf("ap:%f  ", a_pitch);
    yuki.printf("gr:%f\r\n", g_roll);
    // yuki.printf("gp:%f  ", g_pitch);
    // yuki.printf("gy:%f\r\n", g_yaw);
}


