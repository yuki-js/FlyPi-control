#ifndef _PACKETBITS_H_

#define _PACKETBITS_H_
//PB<bit number>_<role>
#define PB1_NOOP 0 //1B
#define PB1_NORMAL 1 //1+4B
#define PB1_QUIT 2//1B
#define PB1_REQUEST_SENSOR 3//1B

#define O_PB1_SENSOR 3 //1+14Bytes

#endif // _PACKETBITS_H_
