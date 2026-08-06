#ifndef __DRV_E2PROM_H__
#define __DRV_E2PROM_H__

#define E2PROM_SIZE         256    /* 总容量(字节) */
#define E2PROM_PAGE_SIZE    8      /* 页大小 */
#define E2PROM_WRITE_MS     10     /* 每页写周期 */

void Drv_E2pRom_Init(void);
esp_err_t Drv_E2pRom_Write(uint8_t reg, uint8_t *data, size_t len,int xfer_timeout_ms);
esp_err_t Drv_E2pRom_Read(uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms);

#endif