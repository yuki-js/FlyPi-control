/*
Copyright (c) <year> yuki-js
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
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
