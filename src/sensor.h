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
#define I2C_ADDR 0x68
#define POWER_MGMT_1 0x6B

extern int i2cHandle;
extern float curSensorVal[6] = {0,0,0,0,0,0};

int cleanI2c();
int initI2c();
uint16_t readWord(int cmd);
uint8_t readWord2c(int cmd);
void readSensor(float* ret);
void averageSensor(float* ret,int16_t samples);
float acc2radX(const float* in);
float acc2radY(const float* in);
void* sense();


#endif
