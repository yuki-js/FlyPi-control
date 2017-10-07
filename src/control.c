/*
Copyright (c) <year> yuki-js
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <pigpio.h>

#include "control.h"
#include "config.h"

int8_t yaw = 0;
int8_t pitch =0;
int8_t roll=0;
uint8_t thro=0;//これだけunsigned

int cleanGPIO(){
  gpioTerminate();
  
  return -1;
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
