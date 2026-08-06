#ifndef __APP_E2PROM_H__
#define __APP_E2PROM_H__

#include "drv_e2prom.h"
void App_E2pRom_Init(void);
void App_E2pRom_Write(uint8_t reg, uint8_t *data, size_t len,int xfer_timeout_ms);
void App_E2pRom_Read(uint8_t reg, uint8_t *data, size_t len, int xfer_timeout_ms);
#endif