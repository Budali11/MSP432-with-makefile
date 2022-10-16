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
#if !defined MPU6050_H
#define MPU6050_H

#include "msp.h"
#include "user_iic.h"
#include "cstdlib"

typedef struct mpu6050_data
{    
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;

    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double old_Gx,Gx;
    double old_Gy,Gy;
    double old_Gz,Gz;
    float pass_us;

    float Temperature;

    double k_roll, k_pitch, k_yaw;
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
    int read_all(mpu6050_data& rdata);
    void kalman_getAngle(mpu6050_data& rdata);
};


// Kalman structure
typedef struct
{
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
}Kalman_t;

#endif
