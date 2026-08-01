#ifndef __DRV_LED_H__
#define __DRV_LED_H__

typedef enum {
    LED_ON = 0,
    LED_OFF,
} Led_State_e;


typedef enum{
    LED_BLUE = 0,
    LED_RED,
}Led_Color_e;

void Drv_Led_Init(void);
void Drv_Led_Set_State(Led_Color_e color, Led_State_e state);

#endif