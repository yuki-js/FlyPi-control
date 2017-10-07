/*
  Copyright (c) 2017 yuki-js
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pigpio.h>
#include <stdlib.h>

#include "config.h"
#include "packetBits.h"
#include "socket.h"
#include "sensor.h"
#include "control.h"

int manualMode=0;
uint8_t armed = 0;

struct setopt_p setoptData = {0,100,0};//センサー切、レポートレート１秒、PWM周波数0
struct setparam_p setparamData = {1,1,1,0,0,0,1,1,1,1,{0,0,0,0,0,0,0,0},20};//PID定数=1,xyz補正:0,軸補正なし(1倍),モータ補正:0,センサーを平均する回数:20

int cleanSock(){
  close(sock);
  return -1;
}

int initSocket(){
  sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock == -1) {
    perror("could not open socket");
    return -1;
  }
  
#ifdef LISTEN_UDS
  struct sockaddr_un sa = {0};
  sa.sun_family = AF_UNIX;
  strcpy(sa.sun_path, LISTEN_UDS);

  remove(sa.sun_path);
  
  if (bind(sock,(const struct sockaddr*) &sa,sizeof(struct sockaddr_un))==-1) {
    perror("socket bind error");
    return cleanSock();
  }
#else
  struct sockaddr_in sa = {0};
  sa.sin_family = AF_INET;
  sa.sin_port=htons(LISTEN_PORT);
  sa.sin_addr.s_addr=INADDR_ANY;

  if (bind(sock,(const struct sockaddr*) &sa,sizeof(struct sockaddr_in))<0) {
    perror("socket bind error");
    return cleanSock();
  }
#endif

  if (listen(sock, 128) == -1){
    perror("listen error");
    return cleanSock();
  }
  return 0;
}

void* socketThread(){//主にデータを受信するスレッド
  uint8_t temp;
 
  int8_t normalData[4];
  uint8_t manualData[MOTOR_LENGTH];

  while(1){
    puts("Waiting for client...");
    cli=accept(sock,NULL,NULL);
    if (cli<0) {
      perror("failed to accept");
      cleanSock();
      return 0;
    }
    puts("Accepted new client");

    while(1){
      if(recv(cli,&temp,sizeof(uint8_t),0)<1){//先頭1バイトを読み込み、分岐
        perror("Disconnected or error");
        break;
      }
      switch(temp){
      case PB1_NOOP://no-operation byte
        break;
      case PB1_NORMAL:
        
        recv(cli,&normalData,sizeof(int8_t)*4,0);//4byte
        
        yaw=(int8_t)normalData[0];
        pitch=(int8_t)normalData[1];
        roll=(int8_t)normalData[2];

        thro=(uint8_t)normalData[3];
        
        break;
      case PB1_MANUAL:
        
        recv(cli,&manualData,sizeof(uint8_t)*MOTOR_LENGTH,0);//MOTOR_LENGTH(8バイト)
        for(int i=0;i<MOTOR_LENGTH;i++){
          motorList[i].value=manualData[i];
        }
        
        break;

      case PB1_QUIT:
        cleanSock();//データなし
        cleanGPIO();
        exit(0);
        break;
      case PB1_SETOPT://struct setopt_pぶん
        recv(cli,&setoptData,sizeof(struct setopt_p),0);

        for(int i=0;i<MOTOR_LENGTH;i++){
          gpioSetPWMfrequency(motorList[i].pin,setoptData.pwmFreq);
        }
        
        break;
      case PB1_SETPARAM://struct setparam_p分
        recv(cli,&setparamData,sizeof(struct setparam_p),0);
        break;
      case PB1_REQUEST_MOTORS://データなし
        send(cli,&motorList,sizeof(struct motorConfig)*MOTOR_LENGTH,0);
        break;
      case PB1_ARM://データなし
        armed=1;
        break;
      case PB1_DISARM://データなし
        armed=0;
        break;
        //shellは使い勝手がわるいのとバグの温床になると思ったので廃止
      }
    }
    cleanSock();
  }
}

void* sendStat(){//コントローラに状態を通知するスレッド
  while(1){
    struct sendStat_o sendData;
    if(setoptData.sendInterval>0){
      sendData.accX=curSensorVal[0];
      sendData.accY=curSensorVal[1];
      sendData.accZ=curSensorVal[2];
      sendData.gyroX=curSensorVal[3];
      sendData.gyroY=curSensorVal[4];
      sendData.gyroZ=curSensorVal[5];

      for(int i = 0;i<MOTOR_LENGTH;i++){
        sendData.motors[i]=motorList[i].value;
      }

      sendData.yaw=yaw;
      sendData.pitch=pitch;
      sendData.roll=roll;
      sendData.thro=thro;

      sendData.armed=armed;

      send(cli,&sendData,sizeof(struct sendStat_o),0);
      
      usleep(setoptData.sendInterval*10000);
    }else{
      sleep(1);
    }
  }
  return;
}
