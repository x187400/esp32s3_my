#include "drv_spiffs.h"
#include "proj_cfg.h"
#include "sta_ui_app.h"
#include "sta_ui_home.h"

static const char *TAG = "Sta_Ui_App";

/* 返回按键点击：回到首页 */
static void music_back_click_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        Sta_Ui_Home_Show();
    }
}

void Sta_Ui_Music_App(lv_obj_t *page)
{
    /* 内容容器：240×320，垂直排列（图片在上、进度条在下） */
    lv_obj_t *part1 = lv_obj_create(page);
    lv_obj_remove_style_all(part1);              /* 清默认样式，避免默认 padding 干扰 flex 布局 */
    lv_obj_set_size(part1, 240, 320);
    lv_obj_set_pos(part1, 0, 0);

    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);   /* 背景不透明（remove_style_all 后必须设，否则看不到颜色） */
    lv_obj_set_style_bg_color(page, lv_color_hex(0x00ff00), 0);   /* 设容器背景色（注意 API） */

    lv_obj_set_flex_flow(part1, LV_FLEX_FLOW_COLUMN);              /* 主轴=垂直 */
    lv_obj_set_flex_align(part1,
                          LV_FLEX_ALIGN_CENTER,   /* 主轴居中 */
                          LV_FLEX_ALIGN_CENTER,   /* 交叉轴居中 */
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(part1, 24, 0);       /* 图片与进度条之间的垂直间距 */

    /* 图片1（原尺寸 150×143） */
    static lv_image_dsc_t dsc1;
    if (!Drv_Spiffs_Load_Image(IMG_PATH_COMPONENT1, &dsc1)) {
        ESP_LOGE(TAG, "load %s failed", IMG_PATH_COMPONENT1);
        return;
    }
    lv_obj_t *img1 = lv_image_create(part1);
    lv_image_set_src(img1, &dsc1);

    /* 音乐进度条（图片下方）：轨道深灰、指示条青色 */
    lv_obj_t *bar = lv_bar_create(part1);
    lv_obj_set_size(bar, 200, 12);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 60, LV_ANIM_OFF);       /* 示例：当前 60% */
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x333333), 0);       /* 轨道 */
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x00CCFF), LV_PART_INDICATOR); /* 指示条 */
    lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);

    /* 返回按键：左上角（page 直接子对象，最后创建保证在最上层） */
    lv_obj_t *back_btn = lv_button_create(page);
    lv_obj_set_pos(back_btn, 10, 10);
    lv_obj_set_size(back_btn, 64, 36);
    lv_obj_add_event_cb(back_btn, music_back_click_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_lbl = lv_label_create(back_btn);
    lv_label_set_text(back_lbl, "返回");
    lv_obj_set_style_text_font(back_lbl, &lv_font_source_han_sans_sc_16_cjk, 0);
    lv_obj_center(back_lbl);
}