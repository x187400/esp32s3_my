#include "proj_cfg.h"
#include "hal_i2c.h"
#include "board.h"
static const char *TAG = "Hal_I2C";


static i2c_master_dev_handle_t i2c_dev_handle[MAX_I2C_DEV_NUM] = {NULL};

static const char *hal_i2c_dev_str(Hal_I2c_Device_t dev)
{
    switch (dev)
    {
    case I2C_XL9555_DEV:
        return "XL9555";
    case I2C_ES8311_DEV:
        return "ES8311";
    case I2C_E2PROM_DEV:
        return "E2PROM";
    default:
        return "Unknown";
    }
}

/**
 * @brief 对I2C设备进行初始化并添加到I2C总线上
 * @param addr_len 地址长度，7位或10位
 * @param addr     设备地址
 * @param clk_speed 时钟频率
 * @param dev      I2C 设备枚举
 */
void Hal_I2C_Add_dev(i2c_addr_bit_len_t addr_len, uint16_t addr,uint32_t clk_speed,Hal_I2c_Device_t dev)
{
    esp_err_t ret = ESP_OK;
    const i2c_device_config_t i2c_dev_config = {
        .dev_addr_length = addr_len,
        .device_address = addr,
        .scl_speed_hz = clk_speed,
    };
    ret = i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config,&i2c_dev_handle[dev]);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c add %s device faild :%d",hal_i2c_dev_str(dev),ret);
    }
}

/**
 * @brief 向设备指定寄存器写入1字节数据
 * @param dev     I2C 设备枚举
 * @param reg     寄存器地址（8位）
 * @param data    待写入数据指针
 * @param len     数据长度
 * @param timeout 超时时间(ms)
 */
void Hal_I2C_Write(Hal_I2c_Device_t dev, uint8_t reg, const uint8_t *data, size_t len, int xfer_timeout_ms)
{
    esp_err_t ret = ESP_OK;
    uint8_t *write_buf = malloc(len + 1);
    if (write_buf == NULL) {
        ESP_LOGE(TAG, "i2c write %s no mem", hal_i2c_dev_str(dev));
        return;
    }
    write_buf[0] = reg;                     /* 首字节 = 字地址 */
    memcpy(write_buf + 1, data, len);       /* 后跟整页数据 */
    ret = i2c_master_transmit(i2c_dev_handle[dev], write_buf, len + 1, xfer_timeout_ms);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c write %s dev faild :%d", hal_i2c_dev_str(dev), ret);
    }
    free(write_buf);
}

/**
 * @brief 从设备指定寄存器读取数据
 * @param dev     I2C 设备枚举
 * @param reg     寄存器地址（8位）
 * @param data    读取数据缓冲区指针
 * @param len     读取长度
 * @param timeout 超时时间(ms)
 */
void Hal_I2C_Read(Hal_I2c_Device_t dev, uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_transmit_receive(i2c_dev_handle[dev], &reg, 1, data, len, xfer_timeout_ms);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c read %s dev faild :%d",hal_i2c_dev_str(dev),ret);
    }
}
