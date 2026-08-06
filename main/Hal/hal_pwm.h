#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

#include "driver/ledc.h"

typedef struct
{
    uint32_t freq;
    int gpio_num;
    ledc_timer_t timer;
    uint32_t duty;
    ledc_channel_t channel;
}Pwm_Cfg_t;

void Hal_Pwm_Init(Pwm_Cfg_t cfg);
void HAL_Pwm_Set_Duty(ledc_channel_t channel,uint32_t duty);

#endif