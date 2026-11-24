#include "sg.h"

void SG_flag(uint32_t LED, uint8_t flag)
{
    switch (LED) 
    {
        case LED1:
            if (flag) {
                DL_GPIO_setPins(SG_PORT, SG_LED1_PIN);
            } else {
                DL_GPIO_clearPins(SG_PORT, SG_LED1_PIN);
            }
            break;
        
        case LED2:
            if (flag) {
                DL_GPIO_setPins(SG_PORT, SG_LED2_PIN);
            } else {
                DL_GPIO_clearPins(SG_PORT, SG_LED2_PIN);
            }
            break;
        
        case BUZZ:
            if (flag) {
                DL_GPIO_setPins(SG_PORT, SG_BUZZ_PIN);
            } else {
                DL_GPIO_clearPins(SG_PORT, SG_BUZZ_PIN);
            }
            break;
    }
}
