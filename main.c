#include "main.h"
#include "stdio.h"


int8_t cs_flag = 0;
int8_t xj_flag = 0;

int8_t q_flag = 0;

uint8_t oled_buffer[32];
uint8_t oled_flag=0;
uint8_t oled_last=0;
extern uint16_t s;
void OLED_View()
{
    if(oled_last != oled_flag)  //当进行oled显示切换时，进行清屏操作
    {
        OLED_Clear();
        oled_last = oled_flag;
    }
    if(oled_flag == 0)   //
    {
        OLED_ShowString(0,0,(uint8_t *)"OLED_VIEW_0",16);


        OLED_ShowNum(0*8,2,L3,1,16);
        OLED_ShowNum(1*8,2,L2,1,16);
        OLED_ShowNum(2*8,2,L1,1,16);
        OLED_ShowNum(3*8,2,M,1,16);
        OLED_ShowNum(4*8,2,R1,1,16);
        OLED_ShowNum(5*8,2,R2,1,16);
        OLED_ShowNum(6*8,2,R3,1,16);

        OLED_ShowNum(11*8,2,Encoder,4,16);

        OLED_ShowNum(0*8,4,xj_flag,1,16);
        OLED_ShowNum(2*8,4,L_flag,2,16);
        OLED_ShowNum(4*8,4,zx_flag,2,16);
        OLED_ShowNum(14*8,4,q_flag,1,16);
         OLED_ShowNum(12*8,4,t_flag,1,16);

        OLED_ShowString(0,6,(uint8_t *)"E_A:",16);

        sprintf((char *)oled_buffer, "%4d", EncoderA);
        OLED_ShowString(4*8,6,oled_buffer,16);

        OLED_ShowString(8*8,6,(uint8_t *)"E_B:",16);

        sprintf((char *)oled_buffer, "%4d", EncoderB);
        OLED_ShowString(12*8,6,oled_buffer,16);
    
        // sprintf((char *)oled_buffer, "%4d", EncoderB);
        // OLED_ShowString(12*8,6,oled_buffer,16);
        // OLED_ShowString(0,7,(uint8_t *)"WIT Demo",8);

        // OLED_ShowString(0,0,(uint8_t *)"Pitch",8);
        // OLED_ShowString(0,2,(uint8_t *)" Roll",8);
        // OLED_ShowString(0,4,(uint8_t *)"  Yaw",8);

        // OLED_ShowString(16*6,3,(uint8_t *)"Accel",8);
        // OLED_ShowString(17*6,4,(uint8_t *)"Gyro",8);

        // sprintf((char *)oled_buffer, "%-6.1f", wit_data.pitch);
        // OLED_ShowString(5*8,0,oled_buffer,16);
        // sprintf((char *)oled_buffer, "%-6.1f", wit_data.roll);
        // OLED_ShowString(5*8,2,oled_buffer,16);
        // sprintf((char *)oled_buffer, "%-6.1f", wit_data.yaw);
        // OLED_ShowString(5*8,4,oled_buffer,16);

        // sprintf((char *)oled_buffer, "%6d", wit_data.ax);
        // OLED_ShowString(15*6,0,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", wit_data.ay);
        // OLED_ShowString(15*6,1,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", wit_data.az);
        // OLED_ShowString(15*6,2,oled_buffer,8);

        // sprintf((char *)oled_buffer, "%6d", wit_data.gx);
        // OLED_ShowString(15*6,5,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", wit_data.gy);
        // OLED_ShowString(15*6,6,oled_buffer,8);
        // sprintf((char *)oled_buffer, "%6d", wit_data.gz);
        // OLED_ShowString(15*6,7,oled_buffer,8);
    }
    else if(oled_flag == 1)      //界面1显示
    {
        OLED_ShowString(0,0,(uint8_t *)"OLED_VIEW_1",16);

    }
    else if (oled_flag == 2) 
    {
        OLED_ShowString(0,0,(uint8_t *)"OLED_VIEW_2",16);

    }
}

void key()  //按键操作
{
    if(!DL_GPIO_readPins(KEY_PORT,KEY_KEY1_PIN))
    {
        mspm0_delay_ms(10);
        if(!DL_GPIO_readPins(KEY_PORT,KEY_KEY1_PIN))
        {
            oled_flag++;
            oled_flag %= 3;
            while(!DL_GPIO_readPins(KEY_PORT,KEY_KEY1_PIN));
        }
    }
    if(!DL_GPIO_readPins(KEY_PORT,KEY_KEY2_PIN))
    {
        mspm0_delay_ms(10);
        xj_flag++;
        xj_flag %= 2;
        L_flag = 0;
        zx_flag = 0;
        if(!DL_GPIO_readPins(KEY_PORT,KEY_KEY2_PIN))
        {
            while(!DL_GPIO_readPins(KEY_PORT,KEY_KEY2_PIN));
        }
    }
    if(!DL_GPIO_readPins(KEY_PORT,KEY_KEY3_PIN))
    {
        mspm0_delay_ms(10);
        q_flag++;
        q_flag%=6;
        if(!DL_GPIO_readPins(KEY_PORT,KEY_KEY3_PIN))
        {
            while(!DL_GPIO_readPins(KEY_PORT,KEY_KEY3_PIN));
        }
    }
}

int main(void)
{
    SYSCFG_DL_init();
    SysTick_Init();
    // MPU6050_Init();
    OLED_Init();
    // Ultrasonic_Init();
    // BNO08X_Init();
    WIT_Init();
    DL_TimerG_startCounter(TB_PWM_INST);              //PWM定时器初始化
    Encoder_Init();
    
    while (1) 
    {
        key();
        OLED_View();
    }
}

void TIMER_Encoder_Read_INST_IRQHandler(void)  //5ms中断
{
    EncoderA = gEncoderCount_left;   
    EncoderB = gEncoderCount_right;
    Encoder += EncoderA;
    gEncoderCount_right = 0;         //将计数清零
    gEncoderCount_left = 0;
    xj_get();
    if(xj_flag == 1 && zx_flag<q_flag*4)
    {
        xj_ms();
    }
   else
   {
        xj_stop();
   }
}
