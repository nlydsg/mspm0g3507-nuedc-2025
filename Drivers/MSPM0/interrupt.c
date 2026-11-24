#include "ti_msp_dl_config.h"
#include "interrupt.h"
#include "clock.h"
#include "mpu6050.h"
#include "bno08x_uart_rvc.h"
#include "wit.h"
#include "XUN.h"
#include "string.h"

void SysTick_Handler(void)
{
    tick_ms++;
}

#if defined UART_BNO08X_INST_IRQHandler
void UART_BNO08X_INST_IRQHandler(void)
{
    uint8_t checkSum = 0;
    extern uint8_t bno08x_dmaBuffer[19];

    DL_DMA_disableChannel(DMA, DMA_BNO08X_CHAN_ID);
    uint8_t rxSize = 18 - DL_DMA_getTransferSize(DMA, DMA_BNO08X_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_BNO08X_INST) == false)
        bno08x_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_BNO08X_INST);

    for(int i=2; i<=14; i++)
        checkSum += bno08x_dmaBuffer[i];

    if((rxSize == 19) && (bno08x_dmaBuffer[0] == 0xAA) && (bno08x_dmaBuffer[1] == 0xAA) && (checkSum == bno08x_dmaBuffer[18]))
    {
        bno08x_data.index = bno08x_dmaBuffer[2];
        bno08x_data.yaw = (int16_t)((bno08x_dmaBuffer[4]<<8)|bno08x_dmaBuffer[3]) / 100.0;
        bno08x_data.pitch = (int16_t)((bno08x_dmaBuffer[6]<<8)|bno08x_dmaBuffer[5]) / 100.0;
        bno08x_data.roll = (int16_t)((bno08x_dmaBuffer[8]<<8)|bno08x_dmaBuffer[7]) / 100.0;
        bno08x_data.ax = (bno08x_dmaBuffer[10]<<8)|bno08x_dmaBuffer[9];
        bno08x_data.ay = (bno08x_dmaBuffer[12]<<8)|bno08x_dmaBuffer[11];
        bno08x_data.az = (bno08x_dmaBuffer[14]<<8)|bno08x_dmaBuffer[13];
    }
    
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_BNO08X_INST, dummy, 4);

    DL_DMA_setDestAddr(DMA, DMA_BNO08X_CHAN_ID, (uint32_t) &bno08x_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_BNO08X_CHAN_ID, 18);
    DL_DMA_enableChannel(DMA, DMA_BNO08X_CHAN_ID);
}
#endif

#if defined UART_WIT_INST_IRQHandler
void UART_WIT_INST_IRQHandler(void)
{
    uint8_t checkSum, packCnt = 0;
    extern uint8_t wit_dmaBuffer[44];

    DL_DMA_disableChannel(DMA, DMA_WIT_CHAN_ID);
    uint8_t rxSize = 43 - DL_DMA_getTransferSize(DMA, DMA_WIT_CHAN_ID);

    if(DL_UART_isRXFIFOEmpty(UART_WIT_INST) == false)
        wit_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_WIT_INST);

    while(rxSize >= 11)
    {
        checkSum=0;
        for(int i=packCnt*11; i<(packCnt+1)*11-1; i++)
            checkSum += wit_dmaBuffer[i];

        if((wit_dmaBuffer[packCnt*11] == 0x55) && (checkSum == wit_dmaBuffer[packCnt*11+10]))
        {
            if(wit_dmaBuffer[packCnt*11+1] == 0x51)
            {
                wit_data.ax = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 2.048; //mg
                wit_data.ay = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 2.048; //mg
                wit_data.az = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 2.048; //mg
                wit_data.temperature =  (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]) / 100.0; //°C
            }
            else if(wit_dmaBuffer[packCnt*11+1] == 0x52)
            {
                wit_data.gx = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 16.384; //°/S
                wit_data.gy = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 16.384; //°/S
                wit_data.gz = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 16.384; //°/S
            }
            else if(wit_dmaBuffer[packCnt*11+1] == 0x53)
            {
                wit_data.roll  = (int16_t)((wit_dmaBuffer[packCnt*11+3]<<8)|wit_dmaBuffer[packCnt*11+2]) / 32768.0 * 180.0; //°
                wit_data.pitch = (int16_t)((wit_dmaBuffer[packCnt*11+5]<<8)|wit_dmaBuffer[packCnt*11+4]) / 32768.0 * 180.0; //°
                wit_data.yaw   = (int16_t)((wit_dmaBuffer[packCnt*11+7]<<8)|wit_dmaBuffer[packCnt*11+6]) / 32768.0 * 180.0; //°
                wit_data.version = (int16_t)((wit_dmaBuffer[packCnt*11+9]<<8)|wit_dmaBuffer[packCnt*11+8]);
            }
        }

        rxSize -= 11;
        packCnt++;
    }
    
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_WIT_INST, dummy, 4);

    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &wit_dmaBuffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 43);
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);
}
#endif


#if defined UART_XJ_INST_IRQHandler

uint8_t flage;
uint8_t xj_sj[8];
uint8_t xj_cans = 0;
void UART_XJ_INST_IRQHandler(void)
{
    uint8_t rebuf= DL_UART_receiveData(UART_XJ_INST);
    if(rebuf == '$' && flage == 0)
    {
        flage = 1;
    }
    if(rebuf == ':' && flage == 1)
    {
        flage = 2;
    }
    else if(rebuf == '#' && flage == 1)
    {
        flage= 0 ;
        xj_cans = 0;
        xj_data.x1 = xj_sj[0];
        xj_data.x2 = xj_sj[1];
        xj_data.x3 = xj_sj[2];
        xj_data.x4 = xj_sj[3];
        xj_data.x5 = xj_sj[4];
        xj_data.x6 = xj_sj[5];
        xj_data.x7 = xj_sj[6];
        xj_data.x8 = xj_sj[7];
    }

    if(flage == 2)
    {
        xj_sj[xj_cans] = rebuf-30;
        xj_cans++;
    }
}


#endif


#if defined UART_GY53_INST_IRQHandler
uint8_t date[5]={},Receive_ok=0;
uint16_t distance,s;
uint8_t i = 0, rebuf;
uint8_t flage = 0;
void UART_GY53_INST_IRQHandler(void)
{
    
    // 读取串口数据并清除接收标志
    rebuf= DL_UART_receiveData(UART_GY53_INST);
    // 检查帧头
   if(flage==0&&rebuf==0x15)
   {
        flage = 1;
   }
   if(flage==1&&rebuf==0x03)
   {
    flage = 2;
   }
   if(flage==2)
   {
        date[i]=rebuf;
        DL_UART_Main_transmitData(UART_GY53_INST,date[i]);
        i++;
        if(i>3)
        {
            if(rebuf==0x06)
            {
                distance = (date[1]<<8)|date[2];
                s=distance;
            }
            else if (rebuf!=0x06)
            {
                s=3000;
            }
            i=0;
            flage=0;
        }
    }
}
#endif

// void GROUP1_IRQHandler(void)
// {
//     switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
//         /* MPU6050 INT */
//         #if defined GPIO_MPU6050_PORT
//             #if defined GPIO_MPU6050_INT_IIDX
//             case GPIO_MPU6050_INT_IIDX:
//             #elif (GPIO_MPU6050_PORT == GPIOA) && (defined GPIO_MULTIPLE_GPIOA_INT_IIDX)
//             case GPIO_MULTIPLE_GPIOA_INT_IIDX:
//             #elif (GPIO_MPU6050_PORT == GPIOB) && (defined GPIO_MULTIPLE_GPIOB_INT_IIDX)
//             case GPIO_MULTIPLE_GPIOB_INT_IIDX:
//             #endif
//                 Read_Quad();
//                 break;
//         #endif
//     }
// }
