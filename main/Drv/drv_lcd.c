#include "proj_cfg.h"
#include "esp_lcd_io_i80.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"   // esp_lcd_panel_* 系列
#include "esp_heap_caps.h"       // MALLOC_CAP_DMA / heap_caps_get_free_size
#include "esp_memory_utils.h"    // esp_ptr_external_ram
#include "driver/gpio.h"         // RD 引脚置高
#include "esp_lv_adapter.h"
#include "drv_lcd.h"

static esp_lcd_i80_bus_handle_t lcd_bus_handle = NULL;
static esp_lcd_panel_io_handle_t lcd_io_handle = NULL;
static esp_lcd_panel_handle_t lcd_panel_handle = NULL;
static const char *TAG = "Drv_LCD";

void Drv_Lcd_Init(void)
{
    esp_err_t ret;

    /* i80 写模式：RD 引脚必须拉高，否则面板不处于写模式，数据无效（雪花屏常见原因） */
    gpio_set_direction(LCD_RD_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LCD_RD_PIN, 1);

    esp_lcd_i80_bus_config_t lcd_bus_cfg = {
        .dc_gpio_num = LCD_DC_PIN,
        .wr_gpio_num = LCD_WR_PIN,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            LCD_D0_PIN,
            LCD_D1_PIN,
            LCD_D2_PIN,
            LCD_D3_PIN,
            LCD_D4_PIN,
            LCD_D5_PIN,
            LCD_D6_PIN,
            LCD_D7_PIN,
        },
        .bus_width = LCD_MAX_BUS_WIDTH,
        .max_transfer_bytes = LCD_MAX_WIDTH * LCD_MAX_HEIGHT * sizeof(uint16_t),
        .dma_burst_size = 64,
    };
    ret = esp_lcd_new_i80_bus(&lcd_bus_cfg,&lcd_bus_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"lcd bus init faild %x",ret);
    }

    esp_lcd_panel_io_i80_config_t lcd_io_cfg = {
        .cs_gpio_num = LCD_CS_PIN,
        .pclk_hz = 10 * 1000 * 1000,   /* 降速提高时序稳定性，排除采样错误 */
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = 0,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ret = esp_lcd_new_panel_io_i80(lcd_bus_handle, &lcd_io_cfg, &lcd_io_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"lcd io init faild %x",ret);
    }

    esp_lcd_panel_dev_config_t lcd_panel_cfg = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ret = esp_lcd_new_panel_st7789(lcd_io_handle, &lcd_panel_cfg,&lcd_panel_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"st7789 pannel init faild %x",ret);
    }
    
    esp_lcd_panel_reset(lcd_panel_handle);
    esp_lcd_panel_init(lcd_panel_handle);
    esp_lcd_panel_invert_color(lcd_panel_handle, true);   // ST7789 常用
    esp_lcd_panel_swap_xy(lcd_panel_handle, true);        // 物理竖屏240x320 → 逻辑横屏320x240（i80旋转必须在此做）
    esp_lcd_panel_mirror(lcd_panel_handle, false, true);
    esp_lcd_panel_set_gap(lcd_panel_handle, 0, 0);
    esp_lcd_panel_disp_on_off(lcd_panel_handle, true);
    /* LCD 硬件初始化到此结束，画面渲染交给 LVGL（见 Drv_Lvgl_Init） */
}

lv_display_t *Drv_Lvgl_Init(void)
{
    /* 1. 初始化 LVGL 适配器（默认 8KB 任务栈，自动创建 LVGL worker 任务） */
    esp_lv_adapter_config_t cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG();
    ESP_ERROR_CHECK(esp_lv_adapter_init(&cfg));

    /* 2. 注册显示设备：
         - i80 接口属于 "OTHER" 类，已启用 PSRAM → 用 WITH_PSRAM 配置宏（显存分配在 PSRAM）
         - 分辨率 320x240（Drv_Lcd_Init 里 swap_xy 后的横屏）
         - 旋转已在 LCD 初始化阶段完成，这里用 ROTATE_0 */
    ESP_LOGI(TAG, "[MEM] before register: internal free=%d, psram free=%d",
             heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
             heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    esp_lv_adapter_display_config_t disp_cfg =
        ESP_LV_ADAPTER_DISPLAY_SPI_WITH_PSRAM_DEFAULT_CONFIG(
            lcd_panel_handle,
            lcd_io_handle,
            LCD_MAX_HEIGHT,
            LCD_MAX_WIDTH,
            ESP_LV_ADAPTER_ROTATE_0);
    lv_display_t *disp = esp_lv_adapter_register_display(&disp_cfg);
    assert(disp != NULL);

    /* 显存验证：register 返回即显存已分配完成。
       对比前后 internal/psram 空闲量，并直接检查 draw buffer 指针是否在 PSRAM */
    ESP_LOGI(TAG, "[MEM] after register: internal free=%d, psram free=%d",
             heap_caps_get_free_size(MALLOC_CAP_INTERNAL),
             heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    lv_draw_buf_t *dbuf = lv_display_get_buf_active(disp);
    if (dbuf && dbuf->data) {
        ESP_LOGI(TAG, "[MEM] draw buffer addr=%p, in PSRAM=%d", dbuf->data, esp_ptr_external_ram(dbuf->data));
    }

    /* 3. 启动适配器任务（内部周期性调用 lv_timer_handler 驱动渲染） */
    ESP_ERROR_CHECK(esp_lv_adapter_start());
    /* UI 内容由 App 层在启动后创建（如 App_Ui_Demo_Show），本层只负责硬件与适配器 */

    return disp;
}
