#include "proj_cfg.h"
#include "hal_spi.h"

static const char *TAG = "Hal_Spi";

static spi_device_handle_t spi_dev_handle = NULL;

void Hal_Spi_Add_Dev(int cs_pin)
{
    esp_err_t ret = ESP_OK;
    spi_device_interface_config_t spi_cfg = {
        .clock_speed_hz = 40000000,
        .spics_io_num = cs_pin,
        .queue_size = 7,
        .mode = 0,
    };
    ret = spi_bus_add_device(SPI2_HOST,&spi_cfg,&spi_dev_handle);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"spi add device faild :%x",ret);
    }
}

void Hal_Spi_Remove_Device(void)
{
    spi_bus_remove_device(spi_dev_handle);
}