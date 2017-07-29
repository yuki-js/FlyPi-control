#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <pigpio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "config.h"
#include "packetBits.h"

int manualMode=0;

unsigned char yaw;
unsigned char pitch;
unsigned char roll;
unsigned char thro;

int sock;
int i2cHandle;

int cleanSock();
int cleanGPIO();
int initSocket(){
  sock = socket(AF_INET,SOCK_STREAM,0);
  if (sock == -1) {
    perror("could not open socket");
    return -1;
  }
  struct sockaddr_in sa = {0};
  sa.sin_family = AF_INET;
  sa.sin_port=htons(33400);
  sa.sin_addr.s_addr=INADDR_ANY;
  
  if (bind(sock,(const struct sockaddr*) &sa,sizeof(struct sockaddr_in))==-1) {
    perror("socket bind error");
    return cleanSock();
  }
  if (listen(sock, 128) == -1){
    perror("listen error");
    return cleanSock();
  }
  return 0;
}
int cli;
int sendSensorVal=0;
void* server(){
 int msgLen;
 unsigned char temp;
 
 unsigned char normalData[4];
  while(1){
    puts("Waiting for client...");
    cli=accept(sock,NULL,NULL);
    if (cli<0) {
      perror("failed to accept");
      cleanSock();
      return 0;
    }
    puts("Accepted new client");

    send(cli,&motorList,sizeof(struct motorConfig)*MOTOR_LENGTH,0);    

    while(1){
      if(recv(cli,&temp,sizeof(char),0)<1){
        perror("disconnected or error");
        break;
      }
      printf("%d\n",temp);
      switch(temp){
      case PB1_NOOP:
        break;
      case PB1_NORMAL:
        
        recv(cli,&normalData,sizeof(char)*4,0);
        yaw=normalData[0];
        pitch=normalData[1];
        roll=normalData[2];
        thro=normalData[3];
        
        break;
      case PB1_QUIT:
        cleanSock();
        cleanGPIO();
        exit(0);
        break;
      case PB1_REQUEST_SENSOR:
        sendSensorVal=!sendSensorVal;
        break;
      }
    }
  }
  cleanSock();
  return ;
}
int cleanSock(){
  close(sock);
  return -1;
}
struct floatVec3{
  float x;
  float y;
  float z;
};

struct charVec3{
  char x;
  char y;
  char z;
};
struct intVec3{
  int x;
  int y;
  int z;
};
struct sensorVal{
  int accX;
  int accY;
  int accZ;
  int temp;
  int gyroX;
  int gyroY;
  int gyroZ;
};
struct sensorVal getSensorValue(reg){
  struct sensorVal ret;
  
  i2cWriteByte(i2cHandle,0x3B);
  
  ret.accX=i2cReadByteData(i2cHandle,0x3c)<<8|i2cReadByteData(i2cHandle,0x3C);
  ret.accX=(ret.accX>=0x8000)?-((65535 -ret.accX) + 1):ret.accX;
  ret.accY=( i2cReadByteData(i2cHandle,0x3D)<<8|i2cReadByteData(i2cHandle,0x3E));
  ret.accY=(ret.accY>=0x8000)?-((65535 -ret.accY) + 1):ret.accY;
  ret.accZ=(i2cReadByteData(i2cHandle,0x3F)<<8|i2cReadByteData(i2cHandle,0x40));
  ret.accZ=(ret.accZ>=0x8000)?-((65535 -ret.accZ) + 1):ret.accZ;
  ret.temp=i2cReadByteData(i2cHandle,0x41)<<8|i2cReadByteData(i2cHandle,0x42);
  ret.gyroX=i2cReadByteData(i2cHandle,0x43)<<8|i2cReadByteData(i2cHandle,0x44);
  ret.gyroY=i2cReadByteData(i2cHandle,0x45)<<8|i2cReadByteData(i2cHandle,0x46);
  ret.gyroZ=i2cReadByteData(i2cHandle,0x47)<<8|i2cReadByteData(i2cHandle,0x48);
  return ret;
}
struct sensorVal curSensorVal;
void* control(){
  while(1){
    curSensorVal = getSensorValue();
    if(sendSensorVal==1){
      
      send(cli,&curSensorVal,15,0);
    }
  }
}
int initGPIO(){
  printf("sizeof(emum motorType)=%d\n",sizeof(enum motorType));
  
  if(gpioInitialise() <0){
    return -1;
  }
  puts("GPIO initialized");
  int motIt;
  for(motIt=0;motIt<MOTOR_LENGTH;motIt++){
    if(gpioSetMode(motorList[motIt].pin, PI_OUTPUT)!=0){
      printf("failed to set motor %s\n",motorList[motIt].name);
      return cleanGPIO();
    }else{
      printf("Successfully set motor %s\n",motorList[motIt].name);
    }
    if(gpioPWM(motorList[motIt].pin,0)!=0){
      printf("failed to init motor %s\n",motorList[motIt].name);
      return cleanGPIO();
    }else{
      printf("Successfully initialized motor %s\n",motorList[motIt].name);
    }
    
  }
  return 0;
}

int initI2c(){
  i2cHandle=i2cOpen(1,0x68,0);
  if(i2cHandle<0){
    return -1;
  }
  if(i2cWriteByteData(i2cHandle,0x6b,0x00)<0){
    return cleanI2c();
  }
  sleep(0.5);
  if(i2cWriteByteData(i2cHandle,0x19,0x00)<0){
    return cleanI2c();
  }
  
  if(i2cWriteByteData(i2cHandle,0x1a,0x00)<0){
    return cleanI2c();
  }
  if(i2cWriteByteData(i2cHandle,0x1b,0x08)<0){
    return cleanI2c();
  }
  if(i2cWriteByteData(i2cHandle,0x1c,0x00)<0){
    return cleanI2c();
  }
  if(i2cWriteByteData(i2cHandle,0x6b,0x01)<0){
    return cleanI2c();
  }
  printf("i2c init ok\n");
  return 0;
}
int cleanI2c(){
  i2cClose(i2cHandle);
  printf("i2c failed");
  return -1;
}
int cleanGPIO(){
  gpioTerminate();
  
  return -1;
}
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
  
  pthread_t commThread;
  pthread_t ctrlThread;
  if(pthread_create(&commThread,NULL,server,NULL)<0
   ||pthread_create(&ctrlThread,NULL,control,NULL)<0){
    perror("failed to create thread\n");
    return 1;
  }
  while(1){
    printf("control:%d %d %d %d\n",yaw,pitch,roll,thro);
    printf("i2c:%f,%f,%f",((float)curSensorVal.accX)/16384.0,((float)curSensorVal.accY)/16384.0,((float)curSensorVal.accZ)/16384.0);
    sleep(1);
  }
  return 0;
}
