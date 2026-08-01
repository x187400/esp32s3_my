#include "pin_cfg.h"
#include "esp_log.h"
#include "board.h"

static const char *TAG = "Board";

i2c_master_bus_handle_t i2c_bus_handle = NULL;

void I2C_Init(void)
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
}