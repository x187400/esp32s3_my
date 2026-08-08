#include "proj_cfg.h"
#include "esp_lcd_io_i80.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"   // esp_lcd_panel_* 系列
#include "esp_heap_caps.h"       // MALLOC_CAP_DMA
#include "driver/gpio.h"         // RD 引脚置高
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
            .swap_color_bytes = 1,
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
    esp_lcd_panel_swap_xy(lcd_panel_handle, true);        // 根据实际旋转需求
    esp_lcd_panel_mirror(lcd_panel_handle, false, true);
    esp_lcd_panel_set_gap(lcd_panel_handle, 0, 0);
    esp_lcd_panel_disp_on_off(lcd_panel_handle, true);
    
    // 测试：整屏刷纯蓝色，验证显示链路
    uint16_t *buf = esp_lcd_i80_alloc_draw_buffer(lcd_io_handle,
                                                  LCD_MAX_WIDTH * LCD_MAX_HEIGHT * sizeof(uint16_t),
                                                  MALLOC_CAP_DMA);
    if (buf == NULL)
    {
        ESP_LOGE(TAG, "lcd alloc draw buffer faild");
        return;
    }
    for (size_t i = 0; i < LCD_MAX_WIDTH * LCD_MAX_HEIGHT; i++)
    {
        buf[i] = 0x001F;   /* RGB565: 蓝色 */
    }
    /* 注意：ST7789 芯片 CASET 上限 239、RASET 上限 319，驱动不自动交换坐标。
        swap_xy 后 x 对应 CASET(垂直,≤240)、y 对应 RASET(水平,≤320)，
        因此必须传 (0,0,高,宽)=(240,320)，否则地址窗口超限导致雪花屏。 */
    ret = esp_lcd_panel_draw_bitmap(lcd_panel_handle, 0, 0, LCD_MAX_WIDTH, LCD_MAX_HEIGHT, buf);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "draw bitmap faild %x", ret);
    }
    else
    {
        ESP_LOGI(TAG, "draw bitmap ok, fill blue");
    }
    /* draw buffer 无官方释放 API，常驻供后续刷新复用 */
}
