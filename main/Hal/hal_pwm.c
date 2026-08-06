
#include "hal_pwm.h"

void Hal_Pwm_Init(Pwm_Cfg_t cfg)
{
    ledc_timer_config_t pwm_timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = cfg.timer,
        .freq_hz = cfg.freq,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false,
    };
    ledc_timer_config(&pwm_timer_cfg);

    ledc_channel_config_t pwm_channel_cfg = {
        .gpio_num = cfg.gpio_num,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = cfg.channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = cfg.timer,
        .duty = 0,
    };
    ledc_channel_config(&pwm_channel_cfg);
}

void HAL_Pwm_Set_Duty(ledc_channel_t channel,uint32_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE,channel,duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE,channel);
}