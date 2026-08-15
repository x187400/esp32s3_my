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

static lv_obj_t *s_page[UI_PAGE_NUM];   /* 页面容器 */
static lv_obj_t *s_dot[UI_PAGE_NUM];    /* 页面指示点 */
static int s_cur_page = 0;              /* 当前页面 */

static void switch_page(int target);    /* 前向声明 */
static void home_img_click_cb(lv_event_t *e);
static void music_page_click_cb(lv_event_t *e);
/* PNG 原始字节缓冲须保持到解码完成，故用 static（PSRAM 优先分配） */
static lv_image_dsc_t s_img_dsc[3];

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

/* ---------------- 页面指示条（小圆点） ---------------- */
static void dots_update(void)
{
    for (int i = 0; i < UI_PAGE_NUM; i++) {
        lv_obj_set_style_bg_color(s_dot[i],
                                  (i == s_cur_page) ? lv_color_hex(0xFFFFFF)
                                                    : lv_color_hex(0x666666), 0);
    }
}

static void dots_create(lv_obj_t *parent)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_remove_style_all(bar);
    lv_obj_set_size(bar, 60, 16);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(bar, 14, 0);

    for (int i = 0; i < UI_PAGE_NUM; i++) {
        s_dot[i] = lv_obj_create(bar);
        lv_obj_remove_style_all(s_dot[i]);
        lv_obj_set_size(s_dot[i], 8, 8);
        lv_obj_set_style_radius(s_dot[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(s_dot[i], lv_color_hex(0x666666), 0);
        lv_obj_set_style_bg_opa(s_dot[i], LV_OPA_COVER, 0);
    }
    dots_update();
}

/* ---------------- 一级页面：标题 + 音乐图标 ---------------- */
static void page_home_create(lv_obj_t *page)
{
    /* 标题：音乐 */
    lv_obj_t *title = lv_label_create(page);
    lv_obj_set_style_text_font(title, &lv_font_source_han_sans_sc_16_cjk, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(title, "音樂");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 14);

    /* 图片：app_music.png（128x128，居中）；点击图标跳转二级页 */
    if (sta_img_load_from_spiffs(IMG_PATH_MUSIC, &s_img_dsc[0])) {
        lv_obj_t *img = lv_image_create(page);
        lv_image_set_src(img, &s_img_dsc[0]);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -6);
        lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(img, home_img_click_cb, LV_EVENT_CLICKED, NULL);
    } else {
        ESP_LOGE(TAG, "load %s failed", IMG_PATH_MUSIC);
    }
}

/* ---------------- 二级页面：两张组件图（竖屏上下排列，contain 缩放） ---------------- */
static void page_music_create(lv_obj_t *page)
{
    const int img_w = 150, img_h = 185;     /* 原始图片尺寸 */
    const int disp_w = 200, disp_h = 130;   /* 目标显示尺寸（竖屏 240x320 内上下排列） */

    /* contain 等比缩放（LVGL scale: 256=100%） */
    int32_t sx = disp_w * 256 / img_w;
    int32_t sy = disp_h * 256 / img_h;
    int32_t scale = (sx < sy) ? sx : sy;

    lv_obj_t *slot1 = lv_obj_create(page);
    lv_obj_remove_style_all(slot1);
    lv_obj_set_size(slot1, disp_w, disp_h);
    lv_obj_align(slot1, LV_ALIGN_TOP_MID, 0, 14);

    lv_obj_t *slot2 = lv_obj_create(page);
    lv_obj_remove_style_all(slot2);
    lv_obj_set_size(slot2, disp_w, disp_h);
    lv_obj_align(slot2, LV_ALIGN_BOTTOM_MID, 0, -14);

    /* 点击二级页内容返回一级页 */
    lv_obj_add_event_cb(slot1, music_page_click_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(slot2, music_page_click_cb, LV_EVENT_CLICKED, NULL);

    if (sta_img_load_from_spiffs(IMG_PATH_COMPONENT1, &s_img_dsc[1])) {
        lv_obj_t *img = lv_image_create(slot1);
        lv_image_set_src(img, &s_img_dsc[1]);
        lv_image_set_scale(img, scale);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    } else {
        ESP_LOGE(TAG, "load %s failed", IMG_PATH_COMPONENT1);
    }

    if (sta_img_load_from_spiffs(IMG_PATH_COMPONENT2, &s_img_dsc[2])) {
        lv_obj_t *img = lv_image_create(slot2);
        lv_image_set_src(img, &s_img_dsc[2]);
        lv_image_set_scale(img, scale);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    } else {
        ESP_LOGE(TAG, "load %s failed", IMG_PATH_COMPONENT2);
    }
}

/* ---------------- 页面切换（直接切换） ---------------- */
static void switch_page(int target)
{
    if (target < 0 || target >= UI_PAGE_NUM || target == s_cur_page) {
        return;
    }

    /* 直接切换：只显示目标页，隐藏其他页（两级界面互斥显示，不并排） */
    for (int i = 0; i < UI_PAGE_NUM; i++) {
        if (i == target) {
            lv_obj_remove_flag(s_page[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(s_page[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

    s_cur_page = target;
    dots_update();
}

/* 点击一级页图标 → 二级页 */
static void home_img_click_cb(lv_event_t *e)
{
    (void)e;
    switch_page(1);
}

/* 点击二级页内容 → 返回一级页 */
static void music_page_click_cb(lv_event_t *e)
{
    (void)e;
    switch_page(0);
}

/* 在 LVGL 适配器启动后调用；内部自动加锁 */
void Sta_Ui_Show(void)
{
    if (esp_lv_adapter_lock(-1) != ESP_OK) {
        return;
    }

    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x101418), 0);

    /* 创建页面容器：两级界面独立，初始只显示一级页（page[1] 隐藏） */
    for (int i = 0; i < UI_PAGE_NUM; i++) {
        s_page[i] = lv_obj_create(scr);
        lv_obj_remove_style_all(s_page[i]);
        lv_obj_set_size(s_page[i], UI_HOR_RES, UI_VER_RES);
        lv_obj_set_pos(s_page[i], 0, 0);
        if (i != 0) {
            lv_obj_add_flag(s_page[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

    page_home_create(s_page[0]);   /* 一级页 */
    page_music_create(s_page[1]);  /* 二级页 */

    /* 底部指示条固定在根屏（z 序高于页面） */
    dots_create(scr);

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
