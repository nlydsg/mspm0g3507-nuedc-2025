#ifndef XUN_H
#define XUN_H

#include "ti_msp_dl_config.h"

extern int8_t L3,L2,L1,M,R1,R2,R3;


void xj_ms(void);
void xj_stop(void);
void xj_get(void);
int Velocity_L(int Target, int encoder_L);
int Velocity_R(int Target, int encoder_R);


extern int16_t Encoder;
extern int16_t L_flag;
extern int16_t R_flag;
extern int16_t EncoderA;
extern int16_t EncoderB;
extern int8_t zx_flag;
extern int8_t t_flag;

#endif
