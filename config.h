#ifndef _CONFIG_H_

#define _CONFIG_H_

#define MOTOR_LENGTH 8 

#define M_X 0b00000000
#define M_Y 0b00000001
#define M_P 0b00000000
#define M_N 0b00000010
#define M_L 0b00000000
#define M_R 0b00000100


struct motorConfig {
  uint8_t pin;
  char name[16];
  int8_t type;
  uint8_t value;
};
struct motorConfig motorList[MOTOR_LENGTH]={
  {24,"FL1",M_X|M_P,0},
  {23,"FL2",M_Y|M_N,0},
  {18,"FR1",M_X|M_P,0},
  {15,"FR2",M_Y|M_P,0},
  {10,"BL1",M_X|M_N,0},
  {27,"BL2",M_Y|M_N,0},
  {22,"BR1",M_X|M_N,0},
  {17,"BR2",M_Y|M_P,0}
};


#endif // _CONFIG_H_
