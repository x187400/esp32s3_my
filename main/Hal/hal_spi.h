#ifndef __HAL_SPI_H__
#define __HAL_SPI_H__

#include "driver/spi_master.h"

void Hal_Spi_Add_Dev(int cs_pin);
void Hal_Spi_Remove_Device(void);

#endif