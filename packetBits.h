#ifndef _PACKETBITS_H_

#define _PACKETBITS_H_
//PB<bit number>_<role>

#define PB1_NOOP 0 //1B
#define PB1_NORMAL 1 //1+4B
#define PB1_QUIT 2//1B
#define PB1_SETOPT 3//1B

struct setopt_p {
  uint8_t sensorEnabled;
  uint8_t sendInterval;//10millisec,zero to disable
  uint16_t pwmFreq;
};

#define PB1_SETPARAM 4

struct setparam_p{
  float kp;
  float ki;
  float kd;
  float xCal;
  float yCal;
  float zCal;
  float yawScale;
  float pitchScale;
  float rollScale;
  float throScale;
  float motorCal[MOTOR_LENGTH];
  uint8_t accelSamples;
};

#define PB1_MANUAL 5
#define PB1_REQUEST_MOTORS 6
#define PB1_ARM 7
#define PB1_DISARM 8
#define PB1_SHELL 9

#define O_PB1_SENSOR 3 //1+14Bytes

struct sendStat_o {
  float accX;
  float accY;
  float accZ;
  float gyroX;
  float gyroY;
  float gyroZ;
  uint8_t motors[MOTOR_LENGTH];
  int8_t yaw;
  int8_t pitch;
  int8_t roll;
  uint8_t thro;
  uint8_t armed;
};

#endif // _PACKETBITS_H_
