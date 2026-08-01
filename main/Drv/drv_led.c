#include "driver/gpio.h"
#include "drv_xl9555.h"
#include "proj_cfg.h"
#include "drv_led.h"

void Drv_Led_Init(void)
{
    esp_err_t ret = ESP_OK;
    const gpio_config_t ledConfig = {
        .pin_bit_mask = (1ULL << LED_BLUE_PIN),  /* 位掩码，必须指定引脚 */
        .mode         = GPIO_MODE_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ret = gpio_config(&ledConfig);
    if(ret != ESP_OK)
    {
        ESP_LOGE("Drv_Led_Init","gpio config faild :%d",ret);
    }
    gpio_set_level(LED_BLUE_PIN, 1);            //初始化蓝色LED为关闭状态

    Drv_XL9555_IO_Cfg(XL9555_LED_R, XL9555_DIR_OUT);
    Drv_XL9555_Set_Val(XL9555_LED_R, 1);        // 初始化红色LED为关闭状态
}

void Drv_Led_Set_State(Led_Color_e color, Led_State_e state)
{
    esp_err_t ret = ESP_OK;
    if(color == LED_BLUE)
    {
        ret = gpio_set_level(LED_BLUE_PIN, state);
        if(ret != ESP_OK)
        {
            ESP_LOGE("Drv_Led_Set_State","gpio set level faild :%d",ret);
        }
    }
    else if(color == LED_RED)
    {
        Drv_XL9555_Set_Val(XL9555_LED_R, state);
    }
}