#ifndef _CONFIG_H_

#define _CONFIG_H_

#define MOTOR_LENGTH 8 

enum motorType {xp,xn,yp,yn};

struct motorConfig {
  char pin;
  char name[16];
  enum motorType type;
};
struct motorConfig motorList[MOTOR_LENGTH]={
  {24,"FL1",xp},
  {23,"FL2",yn},
  {18,"FR1",xp},
  {15,"FR2",yp},
  {10,"BL1",xn},
  {27,"BL2",yn},
  {22,"BR1",xn},
  {17,"BR2",yp}
};


#endif // _CONFIG_H_
