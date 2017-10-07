/*
Fly Pi v2
Copyright (c) 2017 yuki-js
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */
#include <unistd.h>
#include <stdio.h>
#include <pigpio.h>
#include <math.h>


#define EIGHTBIT(x) (uint8_t)((x >255)? 255 : (x < 0)? 0:x)//0-255の範囲の8ビット符号なし整数にする


#include "config.h"
#include "packetBits.h"
#include "sensor.h"
#include "socket.h"
#include "control.h"


int main(int argc,char* argv[]){
  if(initGPIO()!=0){
    perror("GPIO init failed.Exiting...\n");
    return -1;
  }
  if(initI2c()!=0){
    perror("i2c init failed.Exiting...\n");
    return -1;
  }
  if(initSocket()!=0){
    perror("Socket init failed.Exiting...\n");
    return -1;
  }
  
  pthread_t* commThread;
  pthread_t* ctrlThread;
  pthread_t* senseThread;
  pthread_t* sendThread;
  
  commThread = gpioStartThread(socketThread,NULL);
  ctrlThread = gpioStartThread(control,NULL);
  senseThread = gpioStartThread(sense,NULL);
  sendThread = gpioStartThread(sendStat,NULL);
  
  printf("sizeof: setparam_p=%d setopt_p=%d sendStat_o=%d float=%d double=%d uint8_t=%d motorConfig=%d\n",
         sizeof(struct setparam_p),sizeof(struct setopt_p),sizeof(struct sendStat_o),sizeof(float),sizeof(double),sizeof(uint8_t),sizeof(struct motorConfig));
  
  while(1){//メインスレッドは何もしません
    
    sleep(2);
  }
  return 0;
}

