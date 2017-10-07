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
  i2cHandle=i2cOpen(1,I2C_ADDR,0);
  if(i2cHandle<0){
    return -1;
  }
  if(i2cWriteByteData(i2cHandle,POWER_MGMT_1,0)<0){
    return cleanI2c();
  }
  printf("i2c init ok\n");
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
void averageSensor(float* ret,int16_t samples){
  float temp[6]={0,0,0,0,0,0};
  for(uint8_t resetInd=0;resetInd<6;resetInd++){
    ret[resetInd]=0;//ゼロセット
  }
  for(int16_t smpInd=0;smpInd<samples;smpInd++){
    readSensor(temp);
    if(temp[0]==0.0&&temp[1]==0.0&&temp[2]==0.0){//値がゼロになることを軽減するため
      smpInd--;
      continue;
    }
    for(uint8_t setInd=0;setInd<6;setInd++){
      ret[setInd]+=temp[setInd]/(float)samples;//平均の足し合わせ
    }
  }
}
float acc2radX(const float* in){
  return -atan2(in[1],sqrt(in[0]*in[0]+in[2]*in[2]));//センサー値を角度に変換
}
float acc2radY(const float* in){
  return -atan2(in[2],sqrt(in[1]*in[1]+in[0]*in[0]));
}



void* sense(){//センサー値を読み取るスレッド
  while(1){
    if(setoptData.sensorEnabled){
      averageSensor(curSensorVal,setparamData.accelSamples);
    }
  }
}
