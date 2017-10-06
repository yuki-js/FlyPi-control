/*
Copyright (c) <year> yuki-js
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _CONFIG_H_

#define _CONFIG_H_

#define MOTOR_LENGTH 8 

#define M_X 0b00000000//センサX軸が正の時
#define M_Y 0b00000001//負の時
#define M_P 0b00000000//出力を上げる
#define M_N 0b00000010//下げる
#define M_CW 0b00000000//時計回り
#define M_CCW 0b00000100//反時計回り

#define LISTEN_PORT 33400
#define LISTEN_UDS "/tmp/flypi-socket"

struct motorConfig {
  uint8_t pin;//BCM番号。WiringPiや、物理ピンの番号ではない
  char name[16];//16文字まで！！！
  int8_t type;
  uint8_t value;//制御時に使うので、0を必ずセット
};
struct motorConfig motorList[MOTOR_LENGTH]={
  {24,"FL1",M_X|M_P|M_CW,0},
  {23,"FL2",M_Y|M_P|M_CCW,0},
  {15,"FR1",M_X|M_N|M_CW,0},
  {18,"FR2",M_Y|M_P|M_CCW,0},
  {10,"BL1",M_X|M_P|M_CW,0},
  {22,"BL2",M_Y|M_N|M_CCW,0},
  {17,"BR1",M_X|M_N|M_CW,0},
  {27,"BR2",M_Y|M_N|M_CCW,0}
};


#endif // _CONFIG_H_
