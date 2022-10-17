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
    m_accel_fsr = ACCEL_8G_RANGE;
    m_gyro_fsr = GYRO_500D_RANGE;

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

    /* self-test */
    /* send slave address(write) */
    msg[0] = (m_slave_mpu6050->addr << 1)|0;
    msg[1] = ACCEL_XOUT_H_REG;
    data.num = 2;
    ret = ops->write(&data, IIC_NO_STOP);
    if(ret != 2)
    {
        //perror
        yuki.send_string("read accel xout h error.\r\n");
        return -1;
    }
    /* send slave address(read) */
    msg[0] = (m_slave_mpu6050->addr << 1)|1;
    data.num = 1;
    ret = ops->write(&data, IIC_NO_STOP);
    if(ret != 1)
    {
        //perror
        yuki.printf("read accel xout h error.\r\n");
        return -1;
    }

    uint8_t measure[14] = {0};
    data.buf = measure;
    data.num = 14;
    ops->read(&data, IIC_STOP|IIC_ACK);
    for(int i = 0; i < 14; i++)
        yuki.printf("data %d is %d\r\n", i, measure[i]);

    return 0;
}
int MPU6050_T::read_all(mpu6050_data& rdata)
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
    user_clock1 >> rdata.pass_us;

    /* timing */
    user_clock1 = 0;

    /* read out data */
    data.buf = measure;
    data.num = 14;
    ops->read(&data, IIC_STOP|IIC_ACK);
    rdata.Accel_X_RAW = measure[0] << 8 | measure[1];
    rdata.Accel_Y_RAW = measure[2] << 8 | measure[3];
    rdata.Accel_Z_RAW = measure[4] << 8 | measure[5];
    int16_t tmp = measure[6] << 8 | measure[7];
    rdata.Gyro_X_RAW = measure[8] << 8 | measure[9];
    rdata.Gyro_Y_RAW = measure[10] << 8 | measure[11];
    rdata.Gyro_Z_RAW = measure[12] << 8 | measure[13];

    /* computing accel, gyro and temperature */
    rdata.Ax = (double)(rdata.Accel_X_RAW) / accel_LSB[m_accel_fsr];
    rdata.Ay = (double)(rdata.Accel_Y_RAW) / accel_LSB[m_accel_fsr];
    rdata.Az = (double)(rdata.Accel_Z_RAW) / accel_LSB[m_accel_fsr];

    rdata.Temperature = tmp/340.0f + 36.53f;

    rdata.old_Gx = rdata.Gx;
    rdata.old_Gy = rdata.Gy;
    rdata.old_Gz = rdata.Gz;

    rdata.Gx = (double)(rdata.Gyro_X_RAW) / gyro_LSB[m_gyro_fsr];
    rdata.Gy = (double)(rdata.Gyro_Y_RAW) / gyro_LSB[m_gyro_fsr];
    rdata.Gz = (double)(rdata.Gyro_Z_RAW) / gyro_LSB[m_gyro_fsr];

    /* output temp */
    yuki.printf("temp %f\t", rdata.Temperature);
    /* output accel x */
    yuki.printf("Ax: %f\t", rdata.Ax);
    /* output accel y */
    yuki.printf("Ay: %f\t", rdata.Ay);
    /* output accel z */
    yuki.printf("Az: %f\t", rdata.Az);
    /* output gyro x */
    yuki.printf("Gx: %f\t", rdata.Gx);
    /* output gyro y */
    yuki.printf("Gy: %f\t", rdata.Gy);
    /* output gyro z */
    yuki.printf("Gz: %f\t", rdata.Gz);
    /* output delta t */
    yuki.printf("delta t: %fus\r\n",rdata.pass_us);


    return 0;
}
    
void MPU6050_T::kalman_getAngle(mpu6050_data& rdata)
{
    float a_roll, a_pitch;
    float g_roll, g_pitch, g_yaw;

    /* get a_roll and a_pitch according to accel data */
    a_roll = atan(rdata.Ay / rdata.Az);
    a_pitch = -atan(rdata.Ax / (pow(rdata.Ay * rdata.Ay + rdata.Az * rdata.Az, 0.5)));

    /* get g_roll, g_pitch and g_yaw according to gyro data */
    // g_roll = 

}


