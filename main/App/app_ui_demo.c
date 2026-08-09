#include "drv_lcd.h"
#include "drv_touch.h"
#include "lvgl.h"
#include "esp_lv_adapter.h"
#include "app_ui_demo.h"
/* ============================================================================
 * LVGL 概念对照（来自 Qt 背景）
 *   QLabel         -> lv_label_create()          （文字）
 *   QWidget/QFrame -> lv_obj_create()            （基础容器）
 *   QVBoxLayout    -> lv_obj_set_flex_flow(COLUMN)（垂直布局）
 *   QPushButton    -> lv_button_create()          （按钮）
 *   QSS/样式表     -> lv_obj_set_style_*(obj, 值, 0)（对象样式）
 *   signal/slot    -> lv_obj_add_event_cb()        （事件回调）
 * ==========================================================================*/

static void btn_event_cb(lv_event_t *e);

void App_Ui_HardWare_Init(void)
{
    Drv_Lcd_Init();
    Drv_Touch_Init();                     /* 先把 CHSC5432 挂上 I2C 总线 */
    lv_display_t *disp = Drv_Lvgl_Init(); /* 返回真实 display 句柄 */

    esp_lcd_touch_handle_t tp = NULL;
    ESP_ERROR_CHECK(Drv_Touch_Create(&tp));

    esp_lv_adapter_touch_config_t touch_cfg = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, tp);
    lv_indev_t *touch = esp_lv_adapter_register_touch(&touch_cfg);
    assert(touch != NULL);

    App_Ui_Demo_Show();
}

/* 在 LVGL 适配器启动后调用；内部自动加锁 */
void App_Ui_Demo_Show(void)
{
    if (esp_lv_adapter_lock(-1) != ESP_OK) {
        return;
    }

    /* ---------- 屏幕（相当于 QMainWindow 的 central widget）---------- */
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x101418), 0);

    /* ===================== 1. 文字：字号 + 颜色 ===================== */
    /* 标题：24px 字体（需 sdkconfig: CONFIG_LV_FONT_MONTSERRAT_24=y） */
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "LVGL Demo");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x0000FF), 0);  /* 纯蓝 */
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

    /* 16px 白色（需 CONFIG_LV_FONT_MONTSERRAT_16=y） */
    lv_obj_t *l1 = lv_label_create(scr);
    lv_label_set_text(l1, "16px  white");
    lv_obj_set_style_text_font(l1, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(l1, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(l1, LV_ALIGN_TOP_LEFT, 10, 44);

    /* 16px 纯红：相对上一行向下排列 */
    lv_obj_t *l2 = lv_label_create(scr);
    lv_label_set_text(l2, "16px  red");
    lv_obj_set_style_text_font(l2, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(l2, lv_color_hex(0xFF0000), 0);
    lv_obj_align_to(l2, l1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);

    /* 14px 纯绿（默认字体） */
    lv_obj_t *l3 = lv_label_create(scr);
    lv_label_set_text(l3, "14px  green");
    lv_obj_set_style_text_font(l3, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(l3, lv_color_hex(0x00FF00), 0);
    lv_obj_align_to(l3, l2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);

    /* 中文：16px 简体，纯蓝（需 CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_16_CJK=y） */
    lv_obj_t *l4 = lv_label_create(scr);
    lv_label_set_text(l4, "你好 LVGL");
    lv_obj_set_style_text_font(l4, &lv_font_source_han_sans_sc_16_cjk, 0);
    lv_obj_set_style_text_color(l4, lv_color_hex(0x0000FF), 0);      /* 纯蓝 */
    lv_obj_align_to(l4, l3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);

    /* ===================== 2. 容器 + flex 布局 ===================== */
    /* 容器（类似 QWidget/QFrame），放在屏幕底部 */
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, 300, 76);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, -38);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x1E2A38), 0);
    lv_obj_set_style_border_width(cont, 1, 0);
    lv_obj_set_style_border_color(cont, lv_color_hex(0x607D8B), 0);
    lv_obj_set_style_radius(cont, 6, 0);

    /* 垂直自动排列子对象（类似 QVBoxLayout）：
       main=START(主轴从顶部), cross=CENTER(交叉轴居中) */
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(cont, 6, 0);
    lv_obj_set_style_pad_row(cont, 4, 0);   /* 子对象之间的行间距 */

    /* 子对象 1：文字 + 背景色 + 内边距（类似带背景的 QLabel），纯红 */
    lv_obj_t *b1 = lv_label_create(cont);
    lv_label_set_text(b1, "Item 1");
    lv_obj_set_style_bg_color(b1, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_pad_hor(b1, 12, 0);
    lv_obj_set_style_pad_ver(b1, 1, 0);
    lv_obj_set_style_radius(b1, 4, 0);

    /* 子对象 2：纯绿 */
    lv_obj_t *b2 = lv_label_create(cont);
    lv_label_set_text(b2, "Item 2");
    lv_obj_set_style_bg_color(b2, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_pad_hor(b2, 12, 0);
    lv_obj_set_style_pad_ver(b2, 1, 0);
    lv_obj_set_style_radius(b2, 4, 0);

    /* ===================== 3. 按钮 + 事件 ===================== */
    lv_obj_t *btn = lv_button_create(scr);      /* 类似 QPushButton */
    lv_obj_set_size(btn, 76, 30);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -8, -6);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x0000FF), 0);   /* 纯蓝 */

    lv_obj_t *btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, "Press");
    lv_obj_center(btn_lbl);

    /* 事件回调（类似 Qt 的 clicked 信号）：
       事件在 LVGL worker 任务里派发，回调中可直接调用 LVGL API */
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

    esp_lv_adapter_unlock();
}

/* 按钮点击回调 */
static void btn_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *lbl = lv_obj_get_child(btn, 0);   /* 按钮里的 label */
    lv_label_set_text(lbl, "OK!");
}
