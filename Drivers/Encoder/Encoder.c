#include "ti_msp_dl_config.h"

uint32_t gpioA,gpioB;
int16_t gEncoderCount_left,gEncoderCount_right;


void Encoder_Init()
{
    NVIC_EnableIRQ(EncoderA_INT_IRQN);  //将gpio的中断初始化
    NVIC_EnableIRQ(EncoderB_INT_IRQN);
    NVIC_EnableIRQ(TIMER_Encoder_Read_INST_INT_IRQN);
    DL_TimerG_startCounter(TIMER_Encoder_Read_INST);
}

/********************************************
函数功能：中断读取编码器
参数：无
返回值：无
********************************************/
void GROUP1_IRQHandler(void)                    //中断服务函数
{
    /**********************编码器读取***********************/
    gpioA = DL_GPIO_getEnabledInterruptStatus(EncoderA_PORT,EncoderA_A_A_PIN | EncoderA_A_B_PIN);
    gpioB = DL_GPIO_getEnabledInterruptStatus(EncoderB_PORT,EncoderB_B_A_PIN | EncoderB_B_B_PIN);
    if((gpioA & EncoderA_A_A_PIN) == EncoderA_A_A_PIN)
    {
        //Pin0上升沿
        if(!DL_GPIO_readPins(EncoderA_PORT,EncoderA_A_B_PIN))//P1为高电平
        {
            gEncoderCount_left--;
        }
        else//P1为低电平
        {
            gEncoderCount_left++;
        }
    }
    else if((gpioA & EncoderA_A_B_PIN) == EncoderA_A_B_PIN)
    {
        //Pin1上升沿
        if(!DL_GPIO_readPins(EncoderA_PORT,EncoderA_A_A_PIN))//P0为高电平
        {
            gEncoderCount_left++;
        }
        else//P1为低电平
        {
            gEncoderCount_left--;
        }
    }
 
    if((gpioB & EncoderB_B_A_PIN) == EncoderB_B_A_PIN)
    {
        if(!DL_GPIO_readPins(EncoderB_PORT,EncoderB_B_B_PIN))
        {
            gEncoderCount_right++;
        }
        else
        {
            gEncoderCount_right--;
        }
    }
    else if((gpioB & EncoderB_B_B_PIN) == EncoderB_B_B_PIN)
    {
        if(!DL_GPIO_readPins(EncoderB_PORT,EncoderB_B_A_PIN))
        {
            gEncoderCount_right--;
        }
        else
        {
            gEncoderCount_right++;
        }
    }
    DL_GPIO_clearInterruptStatus(GPIOA, EncoderA_A_A_PIN|EncoderA_A_B_PIN);
    DL_GPIO_clearInterruptStatus(GPIOB, EncoderB_B_A_PIN|EncoderB_B_B_PIN);
}
