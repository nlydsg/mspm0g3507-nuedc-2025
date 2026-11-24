#include "XUN.h"
#include "tb.h"
#include "Encoder.h"

int16_t EncoderA;
int16_t EncoderB;

int16_t Encoder;

int8_t zx_flag = 0;
int8_t t_flag = 0;

int8_t sd_L,sd_R;
int8_t stop_L,stop_R;

int16_t L_flag = 0;
int16_t R_flag = 0;


int8_t L3,L2,L1,M,R1,R2,R3;

void xj_get()
{
    L3 = (DL_GPIO_readPins(XJ_L3_PORT,XJ_L3_PIN)!=0)?1:0;
    L2 = (DL_GPIO_readPins(XJ_L2_PORT,XJ_L2_PIN)!=0)?1:0;
    L1 = (DL_GPIO_readPins(XJ_L1_PORT,XJ_L1_PIN)!=0)?1:0;
    M = (DL_GPIO_readPins(XJ_M_PORT,XJ_M_PIN)!=0)?1:0;
    R3 = (DL_GPIO_readPins(XJ_R3_PORT,XJ_R3_PIN)!=0)?1:0;
    R2 = (DL_GPIO_readPins(XJ_R2_PORT,XJ_R2_PIN)!=0)?1:0;
    R1 = (DL_GPIO_readPins(XJ_R1_PORT,XJ_R1_PIN)!=0)?1:0;
}

// 在全局变量区添加
float Line_Kp = 100, Line_Ki = 0, Line_Kd = 0;
int last_error = 0, integral = 0;

//电机pwm值位于-1000到1000，当电机达到最大数度时，编码器读取速度为17
void xj_ms(void)
{
    //1. 计算当前位置误差 (-300~300)
    int position = 0;
    if(L3 || L2 || L1 || M || R1 || R2 || R3) { // 至少有一个传感器看到线
        position = (-3)*L3 + (-2)*L2 + (-1)*L1 + 0*M + 1*R1 + 2*R2 + 3*R3;
    } else {
        // 丢线处理 - 保持最后已知方向
        position = (last_error > 0) ? 1 : -1;
    }
    
    // 2. 位置PID计算
    integral += position;
    integral = (integral < -1000) ? -1000 : ((integral > 1000) ? 1000 : integral); // 积分限幅
    int derivative = position - last_error;
    int correction = Line_Kp * position + Line_Ki * integral + Line_Kd * derivative;
    last_error = position;
    
    if(((L3&&L1)||(L3&&M))&&L_flag==0)
    {
        L_flag = 1;
    }
    if(L_flag != 0)
    {
        L_flag++;
        if(Encoder < 500)
            L_flag = 0;
        if(L_flag > 100)
        {
            if(R1||M||L1)
            {
                if(!t_flag)
                t_flag = 1;
            }
            else
                motor(-400,200);
        }
        else 
        {
            stop_L = 1;
            stop_R = 1;
            motor(100,100);
        }
        if(t_flag != 0)
        {
            t_flag++;
            motor(0, 0);
            if(t_flag > 20)
            {
                L_flag = 0;
                Encoder = 0;
                t_flag = 0;
                zx_flag++;
            }
        }   
    }
    // 5. 应用方向修正
    if(L_flag == 0)
    {
        // 3. 设置基础速度
        sd_L = 8; sd_R = 8;
        // 4. 速度环计算 (修正EncoderA/B混用问题)
        int speed_L = Velocity_L(sd_L, EncoderB);  // 左电机用EncoderB
        int speed_R = Velocity_R(sd_R, EncoderA);  // 右电机用EncoderA

        speed_L += correction;
        speed_R -= correction;

        // 6. PWM限幅
        speed_L = (speed_L < -1000) ? -1000 : ((speed_L > 1000) ? 1000 : speed_L);
        speed_R = (speed_R < -1000) ? -1000 : ((speed_R > 1000) ? 1000 : speed_R);
        
        motor(speed_L, speed_R);
    }
    
    
    
}


//速度环
float Velocity_Kp = 60;
float Velocity_Ki;  

/*
	速度环PI
	输入：期望速度,左编码器,右编码器
*/
int Velocity_L(int Target, int encoder_L)
{
	static int Err_LowOut_last = 0,Encoder_S = 0;
	static float a = 0.7;
	Velocity_Ki = Velocity_Kp / 200;
	int Err,Err_LowOut,temp;
	//1.计算偏差值
	Err = Target - encoder_L;
	//2.低通滤波
	Err_LowOut = (1 - a) * Err + a * Err_LowOut_last;
	Err_LowOut_last = Err_LowOut;
	//3.积分
	Encoder_S += Err_LowOut;
	//4.积分限幅
	Encoder_S = Encoder_S > 10000 ? 10000 : (Encoder_S < (-10000)?(-10000):Encoder_S);
	if(stop_L==1)Encoder_S = 0,stop_L = 0;
	//速度环计算
	temp = Velocity_Kp * Err_LowOut + Velocity_Ki * Encoder_S;
	return temp;
}

int Velocity_R(int Target, int encoder_R)
{
	static int Err_LowOut_last = 0,Encoder_S = 0;
	static float a = 0.7;
	Velocity_Ki = Velocity_Kp / 200;
	int Err,Err_LowOut,temp;
	//1.计算偏差值
	Err = Target - encoder_R;
	//2.低通滤波
	Err_LowOut = (1 - a) * Err + a * Err_LowOut_last;
	Err_LowOut_last = Err_LowOut;
	//3.积分
	Encoder_S += Err_LowOut;
	//4.积分限幅
	Encoder_S = Encoder_S > 10000 ? 10000 : (Encoder_S < (-10000)?(-10000):Encoder_S);
	if(stop_R==1)Encoder_S = 0,stop_R = 0;
	//速度环计算
	temp = Velocity_Kp * Err_LowOut + Velocity_Ki * Encoder_S;
	return temp;
}


void xj_stop()
{
    motor(0,0);
    last_error = 0;
    integral = 0;
    L_flag = 0;
    stop_L = 1;
    stop_R = 1;
}

