#include "drv_lcd.h"
#include "drv_touch.h"
#include "esp_lv_adapter.h"
#include "esp_lv_decoder.h"
#include "lvgl.h"
#include "sta_ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"

/* ============================================================================
 * 竖屏 (240x320) 双页 UI
 *   一级页（Home）：标题「音乐」→ app_music.png → 底部页面指示条（小圆点）
 *   二级页（Music）：app_music1_component / app_music2_component 两张图（上下排列 contain 缩放）
 *   交互：点击一级页图标进入二级页，点击二级页内容返回一级页（直接切换），指示条高亮当前页
 * ==========================================================================*/

#define UI_HOR_RES   240
#define UI_VER_RES   320
#define UI_PAGE_NUM  2

/* SPIFFS 图片路径（spiffs/ 目录随固件烧录） */
#define IMG_PATH_MUSIC      "/spiffs/app_music.png"
#define IMG_PATH_COMPONENT1 "/spiffs/app_music1_component.png"
#define IMG_PATH_COMPONENT2 "/spiffs/app_music2_component.png"

static const char *TAG = "Sta_Ui";

/* 从 SPIFFS 读取 PNG 到内存，构造 LVGL 图像描述符（decoder 会自动识别 PNG magic） */
static bool sta_img_load_from_spiffs(const char *path, lv_image_dsc_t *dsc)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "open %s failed", path);
        return false;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) {
        fclose(f);
        return false;
    }

    uint8_t *buf = heap_caps_malloc(sz, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) {
        buf = malloc(sz);
    }
    if (!buf) {
        fclose(f);
        return false;
    }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) {
        heap_caps_free(buf);
        return false;
    }

    memset(dsc, 0, sizeof(*dsc));
    dsc->header.cf = LV_COLOR_FORMAT_ARGB8888; /* 具体格式由解码器解析 */
    dsc->data = buf;
    dsc->data_size = (uint32_t)sz;
    return true;
}

/* 在 LVGL 适配器启动后调用；内部自动加锁 */
void Sta_Ui_Show(void)
{
    if (esp_lv_adapter_lock(-1) != ESP_OK) {
        return;
    }
    

    esp_lv_adapter_unlock();
}

void Sta_Ui_HardWare_Init(void)
{
    Drv_Lcd_Init();
    Drv_Touch_Init();                     /* 先把 CHSC5432 挂上 I2C 总线 */
    lv_display_t *disp = Drv_Lvgl_Init(); /* 返回真实 display 句柄 */

    esp_lcd_touch_handle_t tp = NULL;
    ESP_ERROR_CHECK(Drv_Touch_Create(&tp));

    esp_lv_adapter_touch_config_t touch_cfg = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, tp);
    lv_indev_t *touch = esp_lv_adapter_register_touch(&touch_cfg);
    assert(touch != NULL);

    /* 注册 esp 图片解码器（PNG/JPEG/QOI），供 lv_image 从内存解码 SPIFFS 图片 */
    esp_lv_decoder_handle_t dec = NULL;
    ESP_ERROR_CHECK(esp_lv_decoder_init(&dec));

    Sta_Ui_Show();
}
