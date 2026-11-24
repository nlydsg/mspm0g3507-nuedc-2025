#include "tb.h"

void motor(int a,int b)
{
    if(b>0) {
        DL_GPIO_setPins(TB_PORT,TB_AIN1_PIN);
        DL_GPIO_clearPins(TB_PORT,TB_AIN2_PIN);
        DL_TimerG_setCaptureCompareValue(TB_PWM_INST,b,DL_TIMER_CC_1_INDEX);
    }
    else {
        DL_GPIO_setPins(TB_PORT,TB_AIN2_PIN);
        DL_GPIO_clearPins(TB_PORT,TB_AIN1_PIN);
        DL_TimerG_setCaptureCompareValue(TB_PWM_INST,-b,DL_TIMER_CC_1_INDEX);
    }
    if (a > 0) {
        DL_GPIO_setPins(TB_PORT,TB_BIN2_PIN);
        DL_GPIO_clearPins(TB_PORT,TB_BIN1_PIN);
       DL_TimerG_setCaptureCompareValue(TB_PWM_INST,a,DL_TIMER_CC_0_INDEX);
    }
    else {
        DL_GPIO_setPins(TB_PORT,TB_BIN1_PIN);
        DL_GPIO_clearPins(TB_PORT,TB_BIN2_PIN);
        DL_TimerG_setCaptureCompareValue(TB_PWM_INST,-a,DL_TIMER_CC_0_INDEX);
    }
    
}
