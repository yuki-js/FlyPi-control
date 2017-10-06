#ifndef _GPIO_H_
#define _GPIO_H_

#define KP setparamData.kp
#define KI setparamData.ki
#define KD setparamData.kd

float ex=0,ey=0,dex=0,dey=0,lx=0,ly=0,pex=0,pey=0,iex=0,iey=0,x=0,y=0;

//４軸のデータ
extern int8_t yaw = 0;
extern int8_t pitch =0;
extern int8_t roll=0;
extern uint8_t thro=0;//これだけunsigned

int initGPIO();
int cleanGPIO();
void* control();

#endif
