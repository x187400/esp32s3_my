#include "hal_i2c.h"
#include <stdlib.h>          // calloc/free
#include <string.h>          // memcpy
#include "freertos/FreeRTOS.h"   // portMUX / portENTER_CRITICAL
#include "proj_cfg.h"
#include "esp_check.h"       // ESP_RETURN_ON_FALSE
#include "esp_log.h"         // ESP_LOGI / ESP_LOGE
#include "drv_touch.h"

static const char *TAG = "Drv_Touch";

void Drv_Touch_Init(void)
{
    Hal_I2C_Add_dev(I2C_ADDR_BIT_LEN_7,TOUCH_ADDR,400000,I2C_TOUCH_DEV);
}

static esp_err_t drv_i2c_touch_read(uint32_t addr, uint8_t *data, size_t len)
{
    return Hal_I2C_Read_Addr32(I2C_TOUCH_DEV, addr, data, len, 100);
}

/* read_data：读 28 字节原始数据，解析后存入 tp->data（框架/适配器会先调用它） */
static esp_err_t drv_chsc5432_read_data(esp_lcd_touch_handle_t tp)
{
    ESP_RETURN_ON_FALSE(tp != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid tp");

    uint8_t buf[CHSC_TOUCH_DATA_LEN];
    esp_err_t ret = drv_i2c_touch_read(CHSC_TOUCH_DATA_REG, buf, CHSC_TOUCH_DATA_LEN);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t event_type = buf[0];        /* 0xFF: 正常触摸 */
    uint8_t finger_num = buf[1];        /* 触点数量 */

    portENTER_CRITICAL(&tp->data.lock);
    if (event_type != CHSC_EVENT_NORMAL || finger_num == 0 || finger_num > CHSC_TOUCH_MAX_POINTS) {
        tp->data.points = 0;            /* 无触摸或数据无效 */
        portEXIT_CRITICAL(&tp->data.lock);
        return ESP_OK;
    }

    uint8_t cnt = finger_num;
    if (cnt > CONFIG_ESP_LCD_TOUCH_MAX_POINTS) {
        cnt = CONFIG_ESP_LCD_TOUCH_MAX_POINTS;
    }

    /* 每点 6 字节：X[7:0] Y[7:0] 压力(保留) Y[11:8]|X[11:8] 事件|ID */
    for (int i = 0; i < cnt; i++) {
        const uint8_t *p = &buf[2 + i * 6];
        tp->data.coords[i].x        = p[0] | ((uint16_t)(p[3] & 0x0F) << 8);
        tp->data.coords[i].y        = p[1] | ((uint16_t)(p[3] >> 4) << 8);
        tp->data.coords[i].strength = 255;                       /* 压力保留 */
        tp->data.coords[i].track_id = p[4] & 0x0F;
    }
    tp->data.points = cnt;
    portEXIT_CRITICAL(&tp->data.lock);

    return ESP_OK;
}

/* get_xy：把 read_data 缓存的数据拷给 LVGL（框架约定，不再重复读） */
static bool drv_chsc5432_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y,
                                uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    ESP_RETURN_ON_FALSE(tp != NULL, false, TAG, "invalid tp");
    ESP_RETURN_ON_FALSE(x != NULL && y != NULL && point_num != NULL, false, TAG, "invalid args");

    portENTER_CRITICAL(&tp->data.lock);
    *point_num = (tp->data.points > max_point_num) ? max_point_num : tp->data.points;
    for (int i = 0; i < *point_num; i++) {
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;
        if (strength) {
            strength[i] = tp->data.coords[i].strength;
        }
    }
    portEXIT_CRITICAL(&tp->data.lock);

    return (*point_num > 0);
}

static esp_err_t drv_chsc5432_del(esp_lcd_touch_handle_t tp)
{
    ESP_RETURN_ON_FALSE(tp != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid tp");
    free(tp);
    return ESP_OK;
}

esp_err_t Drv_Touch_Create(esp_lcd_touch_handle_t *tp)
{
    ESP_RETURN_ON_FALSE(tp != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid tp ptr");

    esp_lcd_touch_handle_t chsc = calloc(1, sizeof(esp_lcd_touch_t));
    ESP_RETURN_ON_FALSE(chsc != NULL, ESP_ERR_NO_MEM, TAG, "no mem for touch");

    /* 验证芯片：读 ictype(4字节)，期望 0x05=CHSC5432 */
    uint8_t ictype[4] = {0};
    if (drv_i2c_touch_read(CHSC_TP_INFO_IC_TYPE, ictype, 4) == ESP_OK) {
        ESP_LOGI(TAG, "CHSC ictype=0x%02X (0x05=CHSC5432)", ictype[0]);
    } else {
        ESP_LOGE(TAG, "read ictype failed, check I2C addr/wiring");
    }

    /* 挂回调 */
    chsc->read_data = drv_chsc5432_read_data;
    chsc->get_xy    = drv_chsc5432_get_xy;
    chsc->del       = drv_chsc5432_del;

    /* 框架要求初始化锁 */
    chsc->data.lock.owner = portMUX_FREE_VAL;

    /* 配置：x_max/y_max 用触摸屏原始范围(竖屏240x320)；
       竖屏：不交换 X/Y、不镜像（与显示方向一致） */
    memcpy(&chsc->config, &(esp_lcd_touch_config_t){
        .x_max = LCD_MAX_WIDTH,         /* 触摸屏 X 原始范围 0~239 */
        .y_max = LCD_MAX_HEIGHT,         /* 触摸屏 Y 原始范围 0~319 */
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,    /* 无 INT 则轮询 */
        .levels = { .reset = 0, .interrupt = 0 },
        .flags = {
            .swap_xy  = 0,   /* 竖屏不交换 X/Y */
            .mirror_x = 0,
            .mirror_y = 0,   /* 竖屏不镜像 */
        },
    }, sizeof(esp_lcd_touch_config_t));

    *tp = chsc;
    return ESP_OK;
}