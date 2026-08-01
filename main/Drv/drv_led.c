#include "driver/gpio.h"
#include "pin_cfg.h"
#include "drv_led.h"


void Drv_Led_Init(void)
{
    const gpio_config_t ledConfig = {
        .pin_bit_mask = (1ULL << LED_BLUE_PIN),  /* 位掩码，必须指定引脚 */
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&ledConfig);
}

void Drv_Led_Set_State(LedState_e state)
{
    gpio_set_level(LED_BLUE_PIN, state);
}