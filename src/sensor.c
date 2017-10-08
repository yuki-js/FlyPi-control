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
  i2cWriteByteData(i2cHandle,MPU6050_SMPLRT_DIV,0x00);// sample rate: 8kHz/(7+1) = 1kHz
  i2cWriteByteData(i2cHandle,MPU6050_CONFIG,0x00);// disable DLPF, gyro output rate = 8kHz
  i2cWriteByteData(i2cHandle,MPU6050_GYRO_CONFIG,0x08); // gyro range: +/- 500dps
  i2cWriteByteData(i2cHandle,MPU6050_ACCEL_CONFIG,0x03);// accel range: +/- 16g
  i2cWriteByteData(i2cHandle,MPU6050_PWR_MGMT_1,0x01);// disable sleep mode, PLL with X gyro
  
  printf("Initialized I2C\n");
  return 0;
}
//https://github.com/emersion/node-i2c-mpu6050を参考にした

uint16_t readWord(int cmd){
  uint8_t high=i2cReadByteData(i2cHandle,cmd);
  uint8_t low=i2cReadByteData(i2cHandle,cmd+1);
  return (high << 8) + low;//上位ビット、下位ビットを結合させる
}

int16_t readWord2c(int cmd){
  uint16_t value = readWord(cmd);

  if (value >= 0x8000) {
		return -((65535 - value) + 1);//補数
	} else {
		return value;
	}
}

void readSensor(float* ret){
  ret[0]=((float)readWord2c(ACCEL_XOUT)/ACCEL_LSB_SENSITIVITY)+setparamData.xCal;
  ret[1]=((float)readWord2c(ACCEL_YOUT)/ACCEL_LSB_SENSITIVITY)+setparamData.yCal;
  ret[2]=((float)readWord2c(ACCEL_ZOUT)/ACCEL_LSB_SENSITIVITY)+setparamData.zCal;
  ret[3]=((float)readWord2c(GYRO_XOUT)/GYRO_LSB_SENSITIVITY);
  ret[4]=((float)readWord2c(GYRO_YOUT)/GYRO_LSB_SENSITIVITY);
  ret[5]=((float)readWord2c(GYRO_ZOUT)/GYRO_LSB_SENSITIVITY);
}
float acc2radX(const float* in){
  return -atan2(in[1],sqrt(in[0]*in[0]+in[2]*in[2]));//センサー値を角度に変換
}
float acc2radY(const float* in){
  return -atan2(in[2],sqrt(in[1]*in[1]+in[0]*in[0]));
}
void complementary(const float* in,float* out,float ratio){
  out[0]=(ratio*in[0] + (1.0-ratio)*in[1]);
  out[1]=(ratio*in[2] + (1.0-ratio)*in[3]);
  out[2]=(ratio*in[4] + (1.0-ratio)*in[5]);
}

static float prev[3] ={0,0,0};
void lpf(const float* in,float* out,float ratio){
  
  out[0]=(ratio*in[0] + (1.0-ratio)*prev[0]);
  out[1]=(ratio*in[1] + (1.0-ratio)*prev[1]);
  out[2]=(ratio*in[2] + (1.0-ratio)*prev[2]);

  prev[0]=in[0];
  prev[1]=in[1];
  prev[2]=in[2];
  
}

void* sense(){//センサー値を読み取るスレッド
  while(1){
    if(setoptData.sensorEnabled){
      readSensor(curSensorVal);
      lpf(curSensorVal,curSensorVal,0.5);
    }
  }
}
