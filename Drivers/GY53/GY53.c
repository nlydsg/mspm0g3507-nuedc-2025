#include "GY53.h"

void GY53_Init()
{
    
    NVIC_ClearPendingIRQ(UART_GY53_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_GY53_INST_INT_IRQN);
}
