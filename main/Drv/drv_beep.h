#ifndef __DRV_BEEP_H__
#define __DRV_BEEP_H__

typedef enum{
    BEEP_ON = 0,
    BEEP_OFF,
}Beep_State_e;

void Drv_Beep_Init(void);
void Drv_Beep_Set_State(Beep_State_e state);

#endif