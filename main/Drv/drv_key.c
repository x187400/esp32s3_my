#include "proj_cfg.h"
#include "drv_xl9555.h"
#include "driver/gpio.h"
#include "drv_key.h"


void Drv_Key_Init(void)
{
    const gpio_config_t keyConfig = {
        .pin_bit_mask = (1ULL << KEY_PIN),  /* 位掩码，必须指定引脚 */
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&keyConfig);

    Drv_XL9555_IO_Cfg(XL9555_KEY0, XL9555_DIR_IN);
    Drv_XL9555_IO_Cfg(XL9555_KEY1, XL9555_DIR_IN);
}

void Drv_Key_Scan(void)
{
    
}