#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "proj_cfg.h"
#include "app_e2prom.h"

void App_E2pRom_Init(void)
{
    Drv_E2pRom_Init();
}

void App_E2pRom_Write(uint8_t reg, uint8_t *data, size_t len,int xfer_timeout_ms)
{
    Drv_E2pRom_Write(reg,data,len,xfer_timeout_ms);
}

void App_E2pRom_Read(uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms)
{
    Drv_E2pRom_Read(reg,data,len,xfer_timeout_ms);
} 

esp_err_t App_E2pRom_Clear_All(void)
{
    uint8_t zero_buf[E2PROM_PAGE_SIZE] = {0};
    uint16_t addr;

    /* 从 0 开始按页写入，天然页对齐，无需跨页拆分 */
    for (addr = 0; addr < E2PROM_SIZE; addr += E2PROM_PAGE_SIZE)
    {
        esp_err_t ret = Drv_E2pRom_Write(addr, zero_buf, E2PROM_PAGE_SIZE, 1000);
        if (ret != ESP_OK) {
            return ret;                         /* 某页失败立即返回，避免继续写坏 */
        }
        vTaskDelay(pdMS_TO_TICKS(E2PROM_WRITE_MS));  /* 等待该页写周期 */
    }
    return ESP_OK;
}