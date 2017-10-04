#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <pigpio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>

#include "config.h"
#include "packetBits.h"

//Fly Pi v2 制御ソフト
//https://github.com/yuki-js/FlyPi-control
//Raspberry Pi Zero Wで動作する。


#define EIGHTBIT(x) (uint8_t)((x >255)? 255 : (x < 0)? 0:x)
//0-255の範囲の8ビット符号なし整数にする


int manualMode=0;
uint8_t armed = 0;//アーム状態(モータのロック)

//４軸のデータ
int8_t yaw = 0;
int8_t pitch =0;
int8_t roll=0;
uint8_t thro=0;//これだけunsigned

int sock;
int i2cHandle;

struct setopt_p  setoptData = {0,100,0};//センサー切、レポートレート１秒、PWM周波数0
struct setparam_p setparamData = {1,1,1,0,0,0,1,1,1,1,{0,0,0,0,0,0,0,0},20};//PID定数=1,xyz補正:0,軸補正なし(1倍),モータ補正:0,センサーを平均する回数:20

//sensor utils

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


//sensor end

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
  sa.sin_port=htons(33400);//デフォルトポートは33400
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

void* server(){//主にデータを受診するスレッド
  int msgLen;
  uint8_t temp;
 
  int8_t normalData[4];
  uint8_t manualData[MOTOR_LENGTH];
  char shellStr[96];


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
        perror("disconnected or error");
        break;
      }
      switch(temp){
      case PB1_NOOP://データなし
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
      case PB1_SHELL://最大96バイトの呼び出したいコマンド
        
        recv(cli,&shellStr,sizeof(char)*96,0);
        system(shellStr);
      }
    }
  }
  cleanSock();
}
int cleanSock(){
  close(sock);
  return -1;
}


float curSensorVal[6] = {0,0,0,0,0,0};
void* sense(){//センサー値を読み取るスレッド
  while(1){
    if(setoptData.sensorEnabled){
      averageSensor(curSensorVal,setparamData.accelSamples);
    }
  }
}



#define KP setparamData.kp
#define KI setparamData.ki
#define KD setparamData.kd

float ex=0,ey=0,dex=0,dey=0,lx=0,ly=0,pex=0,pey=0,iex=0,iey=0,x=0,y=0;

void* control(){//出力を決めるスレッド
  while(1){
    /*
      pitchの値の範囲は -128<pitch<127
      acc2rad()の範囲は -pi<acc2rad()<pi

      最大でpi/6 rad までの傾きにしたい
      すなわちpitch=127 がpi/6 radになる
    */
    if(setoptData.sensorEnabled){//PID制御
      float xRef=(float)pitch*3.14/6/127*setparamData.pitchScale;
      float yRef=(float)roll*3.14/6/127*setparamData.rollScale;
      
      float xVal=acc2radX(curSensorVal);
      float yVal=acc2radY(curSensorVal);

      ex=-xRef+xVal;
      ey=-yRef+yVal;

      dex=ex-lx;
      dey=ey-ly;

      lx=ex;
      ly=ey;
      
      pex = ex;
      pey = ey;

      iex += ex;
      iey += ey;
        
      x=(KP*pex+KI*iex+KD*dex);
      y=(KP*pey+KI*iey+KD*dey);
    }
    for(int i=0;i<MOTOR_LENGTH;i++){
      if(setoptData.sensorEnabled){
        if(((uint8_t)(motorList[i].type&M_Y)) == 0){//モータの種類がYでない=>Xならば
          if(((uint8_t)(motorList[i].type&M_N)) == 0){//Nでない=>Pならば
            motorList[i].value=EIGHTBIT(x+thro*setparamData.throScale);
          }else{
            motorList[i].value=EIGHTBIT(-x+thro*setparamData.throScale);
          }
        }else{
          if(((uint8_t)(motorList[i].type&M_N)) == 0){//P
            motorList[i].value=EIGHTBIT(y+thro*setparamData.throScale);
          }else{
            motorList[i].value=EIGHTBIT(-y+thro*setparamData.throScale);
          }
        }

        if((uint8_t)(motorList[i].type&M_CCW) == 0){//時計回りならば
          motorList[i].value=EIGHTBIT(motorList[i].value+yaw*setparamData.yawScale);
        }else{
          motorList[i].value=EIGHTBIT(motorList[i].value-yaw*setparamData.yawScale);
        }
        
      }
      if(armed){//ロックがかかっていなければ
        gpioPWM(motorList[i].pin,EIGHTBIT(motorList[i].value));
      }
      
    }
   
  }
}

struct sendStat_o sendData;

void* sendStat(){//コントローラに状態を通知するスレッド
  while(1){
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
}


int initGPIO(){  
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
  pthread_t senseThread;
  pthread_t sendThread;
  if(pthread_create(&commThread,NULL,server,NULL)<0
     ||pthread_create(&ctrlThread,NULL,control,NULL)<0
     ||pthread_create(&senseThread,NULL,sense,NULL)<0
     ||pthread_create(&sendThread,NULL,sendStat,NULL)<0){
    perror("failed to create thread\n");
    return 1;
  }
  printf("sizeof: setparam_p=%d setopt_p=%d sendStat_o=%d float=%d double=%d uint8_t=%d motorConfig=%d\n",
         sizeof(struct setparam_p),sizeof(struct setopt_p),sizeof(struct sendStat_o),sizeof(float),sizeof(double),sizeof(uint8_t),sizeof(struct motorConfig));
  while(1){//メインスレッドは何もしません
    /*
      printf("control:%d %d %d %d\n",yaw,pitch,roll,thro);
    printf("raw:");
    for(int i=0;i<MOTOR_LENGTH;i++){
      printf("%d ",motorList[i].value);
    }
    printf("\naccel:%f %f\n",acc2radX(curSensorVal),acc2radY(curSensorVal));
    よくデバッグに使ってた
    */
    sleep(2);
  }
  return 0;
}

