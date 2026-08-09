#ifndef __DRV_SDCARD_H__
#define __DRV_SDCARD_H__

#define MOUNT_POINT     "/0:"

void Drv_Get_SdCard_Usage(size_t *out_total_bytes, size_t *out_free_bytes);
esp_err_t Drv_SD_Card_Init(void);

#endif