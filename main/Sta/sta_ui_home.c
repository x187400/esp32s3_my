#include "drv_lcd.h"
#include "drv_touch.h"
#include "esp_lv_adapter.h"
#include "esp_lv_decoder.h"
#include "sta_ui_home.h"

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


/* SPIFFS 图片路径（spiffs/ 目录随固件烧录） */
#define IMG_PATH_MUSIC      "/spiffs/app_music.png"
#define IMG_PATH_COMPONENT1 "/spiffs/app_music1_component.png"
#define IMG_PATH_COMPONENT2 "/spiffs/app_music2_component.png"

static const char *TAG = "Sta_Ui";

#define CLICK_SLOP  15   /* 判定"点击"的最大位移阈值(像素)，超过视为滑动/拖动 */

static lv_point_t s_press_point = {0, 0};   /* 按下时的触点位置 */

static Sta_Ui_t staUiStruct = {
    .homePage = {NULL},
    .basePage = NULL,
    .navDot = {NULL},
};

static void home_app_click_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        /* 记录按下位置，供松开时判断是否真的"点击"而非滑动 */
        lv_indev_t *indev = lv_indev_active();
        lv_indev_get_point(indev, &s_press_point);
        return;
    }

    if (code != LV_EVENT_CLICKED) {
        return;                          // 过滤事件码
    }

    /* 松开时计算位移：超过阈值说明是滑动/拖动，忽略（限制为仅点击触发） */
    lv_indev_t *indev = lv_indev_active();
    lv_point_t rel_pt;
    lv_indev_get_point(indev, &rel_pt);
    if (abs(rel_pt.x - s_press_point.x) > CLICK_SLOP ||
        abs(rel_pt.y - s_press_point.y) > CLICK_SLOP) {
        ESP_LOGI(TAG, "检测到滑动，忽略点击");
        return;
    }

    /* 确认为点击 */
    lv_obj_t *cur = lv_event_get_current_target(e);   /* 当前触发回调的对象（已设 user_data） */
    Sta_Ui_Home_App_e app = (Sta_Ui_Home_App_e)(uintptr_t)lv_obj_get_user_data(cur);

    switch (app) {
        case HOME_MUSIC:
            ESP_LOGI(TAG, "点击了音乐页");
            break;
        default:
            break;
    }
}

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
    dsc->header.magic = LV_IMAGE_HEADER_MAGIC; /* 标记为"变量图像"，LVGL 才会走 decoder 解析 */
    dsc->header.cf = LV_COLOR_FORMAT_ARGB8888; /* 具体格式由解码器解析 */
    dsc->data = buf;
    dsc->data_size = (uint32_t)sz;
    return true;
}

static void sta_home_struct(lv_obj_t * page)
{
    /* 顶部栏容器 */
    lv_obj_t *contant = lv_obj_create(page);
    lv_obj_remove_style_all(contant);
    lv_obj_set_size(contant, UI_HOR_RES, UI_STRUCT_H);
    lv_obj_set_pos(contant, 0, 0);
    lv_obj_set_style_bg_opa(contant, LV_OPA_COVER, 0);   /* 背景不透明（remove_style_all 后必须设，否则看不到颜色） */
    lv_obj_set_style_bg_color(contant, lv_color_hex(0xff0000), 0);   /* 设容器背景色（注意 API） */

    /* 水平布局：两个 text 横向排列（对应 QHBoxLayout） */
    lv_obj_set_flex_flow(contant, LV_FLEX_FLOW_ROW);                 /* 主轴=横向 */
    lv_obj_set_flex_align(contant,
                          LV_FLEX_ALIGN_SPACE_BETWEEN,  /* 主轴两端对齐：第一个靠最左，最后一个靠最右 */
                          LV_FLEX_ALIGN_CENTER,         /* 交叉轴居中 */
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(contant, 8, 0);     /* 左右内边距 */
    // lv_obj_set_style_pad_column(contant, 12, 0); /* 两个 text 之间的间距 */

    /* 第一个 text：time */
    lv_obj_t *l1 = lv_label_create(contant);
    lv_label_set_text(l1, "time");
    lv_obj_set_style_text_color(l1, lv_color_hex(0xFFFFFF), 0);

    /* 第二个 text：你好（中文需要指定中文字体） */
    lv_obj_t *l2 = lv_label_create(contant);
    lv_label_set_text(l2, "你好");
    lv_obj_set_style_text_font(l2, &lv_font_source_han_sans_sc_16_cjk, 0);
    lv_obj_set_style_text_color(l2, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t *l3 = lv_label_create(contant);
    lv_label_set_text(l3, "我好");
    lv_obj_set_style_text_font(l3, &lv_font_source_han_sans_sc_16_cjk, 0);
    lv_obj_set_style_text_color(l3, lv_color_hex(0xFFFFFF), 0);
}

static void sta_ui_home_music(lv_obj_t *page)
{
    lv_obj_set_style_bg_opa(page, LV_OPA_COVER, 0);   /* 背景不透明（remove_style_all 后必须设，否则看不到颜色） */
    lv_obj_set_style_bg_color(page, lv_color_hex(0x00ff00), 0);   /* 设容器背景色（注意 API） */
    lv_obj_add_flag(page,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(page,home_app_click_cb,LV_EVENT_PRESSED,NULL);   /* 记录按下点 */
    lv_obj_add_event_cb(page,home_app_click_cb,LV_EVENT_CLICKED,NULL);   /* 松开判定点击 */
    /* static：dsc 内部指针指向 malloc 的图片数据，需在图片生命周期内保持有效 */
    static lv_image_dsc_t dsc;
    if (!sta_img_load_from_spiffs(IMG_PATH_MUSIC, &dsc)) {
        ESP_LOGE(TAG, "load %s failed", IMG_PATH_MUSIC);
        return;
    }

    lv_obj_t *img = lv_image_create(page);
    lv_image_set_src(img, &dsc);
    lv_obj_center(img); /* 在页面内居中显示 */
    
}

static void sta_ui_home_app(lv_obj_t *page)
{
    /* 页面内容表：第 i 项 = 第 i 页的内容创建函数（NULL 表示暂无内容） */
    static const page_create_fn_t page_fn[HOME_PAGE_NUM] = {
        sta_ui_home_music,   /* 第 0 页：音乐图标 */
    };

    /* 循环创建并配置所有页面 */
    for (int i = 0; i < HOME_PAGE_NUM; i++) {
        /* 1. 创建页面容器（父=主屏） */
        staUiStruct.homePage[i] = lv_obj_create(page);
        lv_obj_remove_style_all(staUiStruct.homePage[i]);

        /* 2. 统一配置尺寸与位置（图片区，避开顶部状态栏） */
        lv_obj_set_size(staUiStruct.homePage[i], UI_HOR_RES, UI_HOME_H);
        lv_obj_set_pos(staUiStruct.homePage[i], 0, UI_STRUCT_H);
        lv_obj_set_user_data(staUiStruct.homePage[i], (void *)(uintptr_t)i);   /* 绑定页面索引(对应枚举)，供点击回调过滤 */

        /* 3. 非首页默认隐藏（页面互斥显示） */
        if (i != 0) {
            lv_obj_add_flag(staUiStruct.homePage[i], LV_OBJ_FLAG_HIDDEN);
        }

        /* 4. 填充该页内容 */
        if (page_fn[i] != NULL) {
            page_fn[i](staUiStruct.homePage[i]);
        }
    }
}

static void sta_ui_home_navbar(lv_obj_t *page)
{
    lv_obj_t *contant = lv_obj_create(page);
    lv_obj_remove_style_all(contant);
    lv_obj_set_size(contant, UI_HOR_RES, UI_HOME_NAVBAR_H);
    lv_obj_set_pos(contant, 0, UI_STRUCT_H + UI_HOME_H);   /* 导航栏 y=290，正好铺满底部 */
    lv_obj_set_style_bg_opa(contant, LV_OPA_COVER, 0);   /* 背景不透明（remove_style_all 后必须设，否则看不到颜色） */
    lv_obj_set_style_bg_color(contant, lv_color_hex(0x0000ff), 0);   /* 设容器背景色（注意 API） */

    lv_obj_set_flex_flow(contant, LV_FLEX_FLOW_ROW);                 /* 主轴=横向 */
    lv_obj_set_flex_align(contant,
                          LV_FLEX_ALIGN_CENTER,  /* 主轴两端对齐：第一个靠最左，最后一个靠最右 */
                          LV_FLEX_ALIGN_CENTER,         /* 交叉轴居中 */
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_hor(contant, 8, 0);     /* 左右内边距 */

    /* 页面指示栏：固定 3 个圆点，静态默认高亮第一个（当前第一页） */
    lv_obj_set_style_pad_column(contant, 12, 0);   /* 圆点间距 */
    for (int i = 0; i < UI_NAVBAR_DOT_NUM; i++) {
        lv_obj_t *dot = lv_obj_create(contant);
        lv_obj_remove_style_all(dot);
        lv_obj_set_size(dot, 8, 8);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(dot,
                                  (i == 0) ? lv_color_hex(0xFFFFFF) : lv_color_hex(0x666666), 0);
        staUiStruct.navDot[i] = dot;   /* 存指针，供后续切换页面时动态高亮 */
    }
}

static void sta_ui_home(lv_obj_t *page)
{
    sta_home_struct(page);
    sta_ui_home_app(page);
    sta_ui_home_navbar(page);
}

/* 在 LVGL 适配器启动后调用；内部自动加锁 */
void Sta_Ui_Show(void)
{
    if (esp_lv_adapter_lock(-1) != ESP_OK) {
        return;
    }

    staUiStruct.basePage = lv_scr_act();
    lv_obj_set_style_bg_color(staUiStruct.basePage, lv_color_hex(0x101418), 0);
    sta_ui_home(staUiStruct.basePage);
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
