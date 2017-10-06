/*
Copyright (c) <year> yuki-js
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _SOCKET_H_
#define _SOCKET_H_

int sock;
extern int cli;
extern int manualMode=0;
extern uint8_t armed = 0;//アーム状態(モータのロック)

extern struct setopt_p  setoptData = {0,100,0};//センサー切、レポートレート１秒、PWM周波数0
extern struct setparam_p setparamData = {1,1,1,0,0,0,1,1,1,1,{0,0,0,0,0,0,0,0},20};//PID定数=1,xyz補正:0,軸補正なし(1倍),モータ補正:0,センサーを平均する回数:20

int cleanSock();
int initSocket();
void* socketThread();
void* sendStat();

#endif
