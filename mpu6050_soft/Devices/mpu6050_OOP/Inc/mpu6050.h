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
#if !defined MPU6050_OOP_H
#define MPU6050_OOP_H

#include "msp.h"

#if !defined PROCESS_FACE

#include "user_iic.h"
#include "cstdlib"
#include "user_timer32.h"

#define MPU6050_DEBUG              1
#define MPU6050_NO_DEBUG           2
#define MPU6050_NO_COMPUTE         4

// Kalman structure
typedef struct __Kalman
{
    double roll, pitch, yaw;
    double r_K, p_K;
    double re_est, pe_est;

}Kalman_t;

typedef struct mpu6050_data
{
    int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
    int32_t ax_raw_offset, ay_raw_offset, az_raw_offset;
    double Ax, Ay, Az;

    int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
    int32_t gx_raw_offset, gy_raw_offset, gz_raw_offset;
    double Gx, Gy, Gz;

    float pass_us;
    float Temperature;

    Kalman_t k;

}mpu6050_data_t;



// MPU6050 structure
class MPU6050_T
{
private:
    mpu6050_data_t m_data;
    iic_slave *m_slave_mpu6050;
    uint8_t m_accel_fsr, m_gyro_fsr;
public:
    MPU6050_T();
    ~MPU6050_T();
    iic_slave *iic_init(void);
    int device_init(void);
    void read_accel(mpu6050_data& rdata);
    void read_gyro(mpu6050_data& rdata);
    void read_temp(mpu6050_data& rdata);
    int read_all(uint8_t flags);
    void kalman_getAngle(uint8_t flags);
};

#endif

#endif
