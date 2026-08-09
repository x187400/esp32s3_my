#include "proj_cfg.h"
#include "driver/gpio.h"
#include "board.h"

static const char *TAG = "Board";

/* =============== I2C初始化 =============== */
i2c_master_bus_handle_t i2c_bus_handle = NULL;
i2c_int_handler_t i2c_int_handler = NULL;
void Board_I2C_Init(void)
{
    esp_err_t ret = ESP_OK;
    const i2c_master_bus_config_t i2c_bus_cfg = {
        .i2c_port = I2C_PORT,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
    };

    ret = i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c new bus init faild :%d",ret);
    }

    const gpio_config_t i2cIntConfig = {
        .pin_bit_mask = (1ULL << I2C_INT_PIN),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    ret = gpio_config(&i2cIntConfig);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c int gpio config faild :%d",ret);
    }


    ret = gpio_install_isr_service(0);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c int gpio isr service install faild :%d",ret);
    }

    ret = gpio_isr_handler_add(I2C_INT_PIN, i2c_int_handler, NULL);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c int gpio isr add faild :%d",ret);
    }

    // gpio_intr_disable(I2C_INT_PIN);
}

void I2C_Interupt_Callback_Register(i2c_int_handler_t cb)
{
    if(cb != NULL)
    {
        if(i2c_int_handler != NULL)
        {
            ESP_LOGW(TAG,"i2c int callback has been registered, will be replaced");
        }
        else
        {
            i2c_int_handler = cb;
        }
        
    }
}

void Borad_SPI_Init(void)
{
    spi_bus_config_t spi_bus_cfg = {
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .sclk_io_num = SPI_SCK_PIN,
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS,
    };
    spi_bus_initialize(SPI2_HOST,&spi_bus_cfg,SPI_DMA_CH_AUTO);

    
}