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

static mpu6050_data_t data;


int MPU6050_Init(mpu6050_t *pdevice)
{
    pdevice->device_init = MPU6050_Device_Init;
    pdevice->read_all = MPU6050_Read_All;
    pdevice->kalman_getAngle = MPU6050_Kalman_GetAngle;

    return 0;
}

int MPU6050_Device_Init(iic_adapter_t *padapter, mpu6050_t *pdevice)
{
    int ret = 0;
    uint8_t msg[3] = {MPU6050_ADDR_WRITE, WHO_AM_I_REG, 0};
    uint8_t id = 0;
    
    /* check out device id */

    /* send slave address(write) and mpu6050 id register address */
    ret = IIC_Adapter_Write(padapter, msg, 2, IIC_NO_STOP);
    if(ret == 0)
    {
        //perror
        return -1;
    }

    /* send slave address(read) */
    msg[0] = MPU6050_ADDR_READ;
    ret = IIC_Adapter_Write(padapter, msg, 1, IIC_NO_STOP);
    if(ret == 0)
    {
        //perror
        return -1;
    }

    /* read mpu6050 id */
    ret = IIC_Adapter_Read(padapter, &id, 1, IIC_NACK|IIC_STOP);
    if(id != 104)
    {
        //perror
        return -1;
    }

    /* set up power management register */
    /* mpu6050 slave address(write) */
    msg[0] = (MPU6050_ADDR_WRITE);
    msg[1] = PWR_MGMT_1_REG; /* desire register to write */
    msg[2] = 0x00; /* data write into desire register */
    ret = IIC_Adapter_Write(padapter, msg, 3, IIC_STOP);
    if(ret != 3)
    {
        //perror
        return -1;
    }

    /* set up sample rate */
    msg[1] = SMPLRT_DIV_REG;
    msg[2] = 0x07; /* sample rate 1kHz without DLPF */
    //msg[2] = 0x00; /* sample rate 1kHz but need to enable DLPF */
    ret = IIC_Adapter_Write(padapter, msg, 3, IIC_STOP);
    if(ret != 3)
    {
        //perror
        return -1;
    }

    /* configure accelerometer configration register */
    /* set full scale range */
    msg[1] = ACCEL_CONFIG_REG;
    msg[2] = MPU6050_USING_AFSR << 3;
    ret = IIC_Adapter_Write(padapter, msg, 3, IIC_STOP);
    if(ret != 3)
    {
        //perror
        return -1;
    }

    /* configure gyroscope configration register */
    /* set full scale range */
    msg[1] = GYRO_CONFIG_REG;
    msg[2] = MPU6050_USING_GFSR << 3;
    ret = IIC_Adapter_Write(padapter, msg, 3, IIC_STOP);
    if(ret != 3)
    {
        //perror
        return -1;
    }

    /* give up first 300 data */
    for(int i = 0; i < 300; i++)
        MPU6050_Read_All(padapter, pdevice, MPU6050_NO_COMPUTE);

    /* make statistics and average */
    for(uint8_t i = 0; i < 50; i++)
    {
        MPU6050_Read_All(padapter, pdevice, MPU6050_NO_DEBUG);
        /* statistics */
        data.ax_raw_offset += data.Accel_X_RAW;
        data.ay_raw_offset += data.Accel_Y_RAW;
        data.az_raw_offset += (int)accel_LSB[MPU6050_USING_AFSR] - data.Accel_Z_RAW;
        data.gx_raw_offset += data.Gyro_X_RAW;
        data.gy_raw_offset += data.Gyro_Y_RAW;
        data.gz_raw_offset += data.Gyro_Z_RAW;
    }
    /* average */
    data.ax_raw_offset /= 50;
    data.ay_raw_offset /= 50;
    data.az_raw_offset /= 50;
    data.gx_raw_offset /= 50;
    data.gy_raw_offset /= 50;
    data.gz_raw_offset /= 50;

    // yuki.printf("ax_offset: %d\t", m_data.ax_raw_offset);
    // yuki.printf("ay_offset: %d\t", m_data.ay_raw_offset);
    // yuki.printf("az_offset: %d\r\n", m_data.az_raw_offset);

    return 0;
}
int MPU6050_Read_All(iic_adapter_t *padapter, mpu6050_t *pdevice, uint32_t flags)
{
    uint8_t measure[14] = {0};
    uint32_t ret = 0;
    /* specify the first register to be read */
    uint8_t msg[2] = {0};
    msg[0] = (MPU6050_ADDR_WRITE);
    msg[1] = ACCEL_XOUT_H_REG;
    ret = IIC_Adapter_Write(padapter, msg, 2, IIC_NO_STOP);
    if(ret != 2)
    {
        //perror
        return -1;
    }

    /* slave address(read) */
    msg[0] = MPU6050_ADDR_READ;
    ret = IIC_Adapter_Write(padapter, msg, 1, IIC_NO_STOP);
    if(ret != 1)
    {
        //perror
        return -1;
    }

    // /* read out last time timer value */
    // user_clock1 >> m_data.pass_us;

    // /* timing */
    // user_clock1 = 0;

    /* read out data */
    ret = IIC_Adapter_Read(padapter, measure, 14, IIC_STOP|IIC_ACK);

    if((flags & MPU6050_NO_COMPUTE) != 0)
        return 0;

    data.Accel_X_RAW = measure[0] << 8 | measure[1];
    data.Accel_Y_RAW = measure[2] << 8 | measure[3];
    data.Accel_Z_RAW = measure[4] << 8 | measure[5];
    int16_t tmp = measure[6] << 8 | measure[7];
    data.Gyro_X_RAW = measure[8] << 8 | measure[9];
    data.Gyro_Y_RAW = measure[10] << 8 | measure[11];
    data.Gyro_Z_RAW = measure[12] << 8 | measure[13];

    /* computing accel, gyro and temperature */
    data.Ax = (double)(data.Accel_X_RAW + data.ax_raw_offset) / accel_LSB[MPU6050_USING_AFSR];
    data.Ay = (double)(data.Accel_Y_RAW + data.ay_raw_offset) / accel_LSB[MPU6050_USING_AFSR];
    data.Az = (double)(data.Accel_Z_RAW + data.az_raw_offset) / accel_LSB[MPU6050_USING_AFSR];

    data.Temperature = tmp/340.0f + 36.53f;

    data.Gx = (double)(data.Gyro_X_RAW + data.gx_raw_offset) / gyro_LSB[MPU6050_USING_GFSR];
    data.Gy = (double)(data.Gyro_Y_RAW + data.gy_raw_offset) / gyro_LSB[MPU6050_USING_GFSR];
    data.Gz = (double)(data.Gyro_Z_RAW + data.gz_raw_offset) / gyro_LSB[MPU6050_USING_GFSR];
    
    if((flags & MPU6050_NO_DEBUG) != 0)
        return 0;

    /* debug information */

    return 0;
}
void MPU6050_Kalman_GetAngle(mpu6050_t *pdevice, uint32_t flags)
{

}
