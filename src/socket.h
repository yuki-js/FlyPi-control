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
