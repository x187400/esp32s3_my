#include "hal_i2c.h"
#include "proj_cfg.h"
#include "drv_e2prom.h"

static const char *TAG = "Drv_E2pRom";

void Drv_E2pRom_Init(void)
{
    uint8_t data = 0x55;
    Hal_I2C_Add_dev(I2C_ADDR_BIT_LEN_7,E2PROM_ADDR,400000,I2C_E2PROM_DEV);
    Drv_E2pRom_Write(0x01,&data,1,1000);

}

esp_err_t Drv_E2pRom_Write(uint8_t reg, uint8_t *data, size_t len,int xfer_timeout_ms)
{
    return Hal_I2C_Write(I2C_E2PROM_DEV,reg,data,len,xfer_timeout_ms);
}

esp_err_t Drv_E2pRom_Read(uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms)
{
    return Hal_I2C_Read(I2C_E2PROM_DEV,reg,data,len,xfer_timeout_ms);
}