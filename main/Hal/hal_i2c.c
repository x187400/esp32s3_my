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
    case I2C_TOUCH_DEV:
        return "TOUCH";
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
esp_err_t Hal_I2C_Write(Hal_I2c_Device_t dev, uint8_t reg, const uint8_t *data, size_t len, int xfer_timeout_ms)
{
    esp_err_t ret = ESP_OK;
    uint8_t *write_buf = malloc(len + 1);
    if (write_buf == NULL) {
        ESP_LOGE(TAG, "i2c write %s no mem", hal_i2c_dev_str(dev));
        return ESP_ERR_NO_MEM;
    }
    write_buf[0] = reg;                     /* 首字节 = 字地址 */
    memcpy(write_buf + 1, data, len);       /* 后跟整页数据 */
    ret = i2c_master_transmit(i2c_dev_handle[dev], write_buf, len + 1, xfer_timeout_ms);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c write %s dev faild :%d", hal_i2c_dev_str(dev), ret);
    }
    free(write_buf);
    return ret;
}

/**
 * @brief 从设备指定寄存器读取数据
 * @param dev     I2C 设备枚举
 * @param reg     寄存器地址（8位）
 * @param data    读取数据缓冲区指针
 * @param len     读取长度
 * @param timeout 超时时间(ms)
 */
esp_err_t Hal_I2C_Read(Hal_I2c_Device_t dev, uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ret = i2c_master_transmit_receive(i2c_dev_handle[dev], &reg, 1, data, len, xfer_timeout_ms);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"i2c read %s dev faild :%d",hal_i2c_dev_str(dev),ret);
    }
    return ret;
}

esp_err_t Hal_I2C_Read_Addr32(Hal_I2c_Device_t dev, uint32_t addr, uint8_t *data, size_t len, int xfer_timeout_ms)
{
    /* 应用笔记要求两步独立事务（地址写完后 STOP，再单独读），
       不能用 transmit_receive（repeated START，很多触摸芯片不支持）：
       Step1: Start + 写地址 + ADDR[31:24]..ADDR[7:0] + STOP
       Step2: Start + 读地址 + DATA + STOP */
    uint8_t addr_buf[4] = { (uint8_t)(addr >> 24), (uint8_t)(addr >> 16),
                            (uint8_t)(addr >> 8),  (uint8_t)(addr) };
    esp_err_t ret = i2c_master_transmit(i2c_dev_handle[dev], addr_buf, 4, xfer_timeout_ms);
    if (ret == ESP_OK) {
        ret = i2c_master_receive(i2c_dev_handle[dev], data, len, xfer_timeout_ms);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c read addr32 %s faild :%d", hal_i2c_dev_str(dev), ret);
    }
    return ret;
}

esp_err_t Hal_I2C_Write_Addr32(Hal_I2c_Device_t dev, uint32_t addr, const uint8_t *data, size_t len, int xfer_timeout_ms)
{
    uint8_t *buf = malloc(len + 4);
    if (buf == NULL) {
        ESP_LOGE(TAG, "i2c write addr32 %s no mem", hal_i2c_dev_str(dev));
        return ESP_ERR_NO_MEM;
    }
    buf[0] = (uint8_t)(addr >> 24); buf[1] = (uint8_t)(addr >> 16);
    buf[2] = (uint8_t)(addr >> 8);  buf[3] = (uint8_t)(addr);
    memcpy(buf + 4, data, len);
    esp_err_t ret = i2c_master_transmit(i2c_dev_handle[dev], buf, len + 4, xfer_timeout_ms);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "i2c write addr32 %s dev faild :%d", hal_i2c_dev_str(dev), ret);
    }
    free(buf);
    return ret;
}