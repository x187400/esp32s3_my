#include "proj_cfg.h"
#include "driver/gpio.h"
#include "drv_key.h"

static const char *TAG = "Drv_Key";
static bool key_int = false;

static Key_Timer_t keyTimer = {
    .keyScanTimer = NULL,
    .keyStart = false,
};

static Key_Struct_t keyStruct[KEY_NUM];

static void drv_key_int_handler(void *arg)
{
    (void)arg;
    key_int = true;
}

static uint8_t drv_key_read(Key_Index_e idx)
{
    switch (idx)
    {
    case KEY0: return gpio_get_level(KEY_PIN);
    case KEY1: return Drv_XL9555_Get_Val(XL9555_KEY0);
    case KEY2: return Drv_XL9555_Get_Val(XL9555_KEY1);
    default:   return KEY_RELEASE;
    }
}

static void drv_key_scan(void)
{
    static Key_State_e keyEvtCheck;
    uint8_t i;
    for (i = 0; i < KEY_NUM; i++)
    {
        uint8_t level = drv_key_read((Key_Index_e)i);
        if((Key_State_e)level == KEY_PRESS)
        {
            keyStruct[i].pressCount++;

            if(keyStruct[i].pressCount >= KEY_LONG_PRESS_CNT && !keyStruct[i].longPressed)
            {
                keyStruct[i].longPressed = true;
                //触发长按
                ESP_LOGI(TAG,"key%d longPress",i);
            }
        }
        else
        {
            if(keyStruct[i].pressCount > 0)
            {
                if(keyStruct[i].longPressed)
                {
                    //触发长按释放
                    keyStruct[i].longPressed = false;
                    ESP_LOGI(TAG,"key%d longPress Release",i);
                }
                else
                {
                    keyStruct[i].clickCount++;      //短按计数+
                }
                keyStruct[i].pressCount = 0;
            }

            if(keyStruct[i].clickCount >= 2)
            {
                keyStruct[i].clickCount = 0;
                keyStruct[i].releaseCount = 0;
                ESP_LOGI(TAG,"key%d double click",i);
            }
            else if(keyStruct[i].clickCount == 1)
            {
                keyStruct[i].releaseCount++;
                if(keyStruct[i].releaseCount >= KEY_DOUBLE_CLICK_CNT)
                {
                    keyStruct[i].clickCount = 0;
                    keyStruct[i].releaseCount = 0;
                    ESP_LOGI(TAG,"key%d single click",i);
                }
            }
        }
    }
}

static void key_scan_timer_cb(TimerHandle_t xTimer)     //当前触发定时器回调为下降沿
{
    drv_key_scan();
}

void Drv_Key_Init(void)
{
    esp_err_t ret = ESP_OK;
    const gpio_config_t keyConfig = {
        .pin_bit_mask = (1ULL << KEY_PIN),  /* 位掩码，必须指定引脚 */
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&keyConfig);

    Drv_XL9555_IO_Cfg(XL9555_KEY0, XL9555_DIR_IN);
    Drv_XL9555_IO_Cfg(XL9555_KEY1, XL9555_DIR_IN);

    ret = gpio_isr_handler_add(KEY_PIN, drv_key_int_handler, NULL);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c int gpio isr add faild :%d",ret);
    }

    keyTimer.keyScanTimer = xTimerCreate("key_timer",pdMS_TO_TICKS(KEY_SCAN_TIME),pdTRUE,(void *)0,key_scan_timer_cb);
}

void Drv_Key_Scan(void)
{
    if(keyTimer.keyStart != true)
    {
        keyTimer.keyStart = true;
        xTimerStart(keyTimer.keyScanTimer,portMAX_DELAY);
    }
}

bool Drv_XL9555_Key_Int_Trigger_Check(void)
{
    uint8_t key0_val = Drv_XL9555_Get_Val(XL9555_KEY0);
    uint8_t key1_val = Drv_XL9555_Get_Val(XL9555_KEY1);
    
    if(key0_val == 0 || key1_val == 0)
    {
        return true;
    }
    return false;
}

bool Drv_Key_Int_Trigger_Check(void)
{
    if(key_int == true)
    {
        key_int = false;
        return true;
    }
    return false;
}
