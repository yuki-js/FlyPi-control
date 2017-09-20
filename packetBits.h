#ifndef _PACKETBITS_H_

#define _PACKETBITS_H_
//PB1_<role>

#define PB1_NOOP 0 //無視
#define PB1_NORMAL 1 //yaw,pitch,roll,throttleの制御
#define PB1_QUIT 2//プロセス終了
#define PB1_SETOPT 3//オプション

struct setopt_p {
  uint8_t sensorEnabled;//センサーを有効化するか
  uint8_t sendInterval;//状態を通知する間隔。単位は10ミリ秒。0で送信しない
  uint16_t pwmFreq;//PWMの周波数
};

#define PB1_SETPARAM 4//パラメータ

struct setparam_p{
  float kp;//P定数
  float ki;//I
  float kd;//D
  float xCal;//センサ補正値
  float yCal;
  float zCal;
  float yawScale;//軸スケール
  float pitchScale;
  float rollScale;
  float throScale;
  float motorCal[MOTOR_LENGTH];//モータ補正ち
  uint8_t accelSamples;//センサ平均のサンプル数
};

#define PB1_MANUAL 5//モータ値を直接指定
#define PB1_REQUEST_MOTORS 6//モーターの名前、設定値の定数などを返す
#define PB1_ARM 7//アーム　これを送信しなければ回らない
#define PB1_DISARM 8//ディスアーム
#define PB1_SHELL 9//shellコマンド

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
