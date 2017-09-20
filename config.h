#ifndef _CONFIG_H_

#define _CONFIG_H_

#define MOTOR_LENGTH 8 

#define M_X 0b00000000//センサX軸が正の時
#define M_Y 0b00000001//負の時
#define M_P 0b00000000//出力を上げる
#define M_N 0b00000010//下げる
#define M_CW 0b00000000//時計回り
#define M_CCW 0b00000100//反時計回り


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
