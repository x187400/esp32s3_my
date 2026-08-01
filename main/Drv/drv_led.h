#ifndef __DRV_LED_H__
#define __DRV_LED_H__

typedef enum {
    LED_ON = 0,
    LED_OFF,
} LedState_e;

void Drv_Led_Init(void);
void Drv_Led_Set_State(LedState_e state);

#endif