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
#if !defined MPU6050_DEFS_H
#define MPU6050_DEFS_H

#define RAD_TO_DEG 57.295779513082320876798154814105

#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1_REG 0x6B
#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_CONFIG_REG 0x1B
#define GYRO_XOUT_H_REG 0x43

#define MPU6050_ADDR 0x68
#define MPU6050_ADDR_READ 0xD1
#define MPU6050_ADDR_WRITE 0xD0

#define ACCEL_2G_RANGE          0
#define ACCEL_4G_RANGE          1
#define ACCEL_8G_RANGE          2
#define ACCEL_16G_RANGE         3
#define GYRO_250D_RANGE         0
#define GYRO_500D_RANGE         1
#define GYRO_1000D_RANGE        2
#define GYRO_2000D_RANGE        3
#define MPU6050_USING_AFSR      ACCEL_4G_RANGE
#define MPU6050_USING_GFSR      GYRO_500D_RANGE


#define RAD2DEG                 57.2957795131f

float accel_LSB[] = 
{
    16384.0f,
    8192.0f,
    4096.0f,
    2048.0f
};

float gyro_LSB[] = 
{
    131.0f,
    65.5f,
    32.8f,
    16.4f
};





#endif