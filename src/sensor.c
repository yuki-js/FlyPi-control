/*

Copyright (c) 2017 yuki-js
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include <stdio.h>
#include <pigpio.h>
#include <math.h>
#include "socket.h"
#include "sensor.h"

float curSensorVal[6] = {0,0,0,0,0,0};
int i2cHandle = -1;

static int i2cSensorBuffer[14];

int cleanI2c(){
  i2cClose(i2cHandle);
  printf("i2c failed");
  return -1;
}

int initI2c(){
  i2cHandle=i2cOpen(1,MPU6050_ADDR,0);
  if(i2cHandle<0){
    return -1;
  }
  if(i2cWriteByteData(i2cHandle,MPU6050_PWR_MGMT_1,0)<0){
    return cleanI2c();
  }
  if(i2cReadByteData(i2cHandle,MPU6050_WHO_AM_I)!=MPU6050_ADDR){
    return cleanI2c();
  }
  i2cWriteByteData(i2cHandle,MPU6050_SMPLRT_DIV,0x01);// sample rate (investigating)
  i2cWriteByteData(i2cHandle,MPU6050_CONFIG,0x00);// disable DLPF, gyro output rate = 8kHz
  i2cWriteByteData(i2cHandle,MPU6050_GYRO_CONFIG,0x08); // gyro range: +/- 500dps
  i2cWriteByteData(i2cHandle,MPU6050_ACCEL_CONFIG,0x01);// accel range: +/- 4g
  i2cWriteByteData(i2cHandle,MPU6050_PWR_MGMT_1,0x01);// disable sleep mode, PLL with X gyro
  
  printf("Initialized I2C\n");
  return 0;
}
//https://github.com/emersion/node-i2c-mpu6050を参考にした

#define READ_SB(i) (float)((((int16_t)buffer[i]) << 8) | buffer[i+1])

void readSensor(float* ret){
  i2cReadI2CBlockData(i2cHandle,ACCEL_XOUT,i2cSensorBuffer,14);
  
  ret[0]=(READ_SB(0)/ACCEL_LSB_SENSITIVITY)+setparamData.xCal;
  ret[1]=(READ_SB(2)/ACCEL_LSB_SENSITIVITY)+setparamData.yCal;
  ret[2]=(READ_SB(4)/ACCEL_LSB_SENSITIVITY)+setparamData.zCal;
  ret[3]=(READ_SB(8)/GYRO_LSB_SENSITIVITY)+setparamData.xGyroCal;
  ret[4]=(READ_SB(10)/GYRO_LSB_SENSITIVITY)+setparamData.yGyroCal;
  ret[5]=(READ_SB(12)/GYRO_LSB_SENSITIVITY)+setparamData.zGyroCal;
}
float acc2radX(const float* in){
  return -atan2(in[1],sqrt(in[0]*in[0]+in[2]*in[2]));//センサー値を角度に変換
}
float acc2radY(const float* in){
  return -atan2(in[2],sqrt(in[1]*in[1]+in[0]*in[0]));
}
void complementary(float* in,float ratio){
  in[0]=(float)(ratio*in[0] + (1.0-ratio)*in[3]);
  in[1]=(float)(ratio*in[1] + (1.0-ratio)*in[4]);
  in[2]=(float)(ratio*in[2] + (1.0-ratio)*in[5]);
}

void dmp(){
  printf("dmp(): %x",i2cReadByteData(i2cHandle,0x6A));
}

void* sense(){//センサー値を読み取るスレッド
  while(1){
    if(setoptData.sensorEnabled){
      readSensor(curSensorVal);
      complementary(curSensorVal,0.1);
    }
  }
}
