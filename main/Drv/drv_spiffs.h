#ifndef __DRV_SPIFFS_H__
#define __DRV_SPIFFS_H__

#include "esp_err.h"

/* 初始化并挂载 SPIFFS（分区 label: storage，挂载点 /spiffs） */
esp_err_t Drv_Spiffs_Init(void);

#endif
