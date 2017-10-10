#ifndef _I2C_H_
#define _I2C_H_

#define ACCEL_XOUT 0x3B
#define ACCEL_YOUT 0x3D
#define ACCEL_ZOUT 0x3F

#define GYRO_XOUT 0x43
#define GYRO_YOUT 0x45
#define GYRO_ZOUT 0x47

#define ACCEL_LSB_SENSITIVITY (float)16384.0
#define GYRO_LSB_SENSITIVITY (float)16384.0

#define MPU6050_ADDR         0x68 // MPU-6050 device address
#define MPU6050_SMPLRT_DIV   0x19 // MPU-6050 register address
#define MPU6050_CONFIG       0x1a // See https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
#define MPU6050_GYRO_CONFIG  0x1b
#define MPU6050_ACCEL_CONFIG 0x1c
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6b

extern int i2cHandle;
extern float curSensorVal[6];

int cleanI2c();
int initI2c();
uint16_t readWord(int cmd);
int16_t readWord2c(int cmd);
void readSensor(float* ret);
void averageSensor(float* ret,int16_t samples);
float acc2radX(const float* in);
float acc2radY(const float* in);
uint8_t dmp();
void* sense();

#endif
