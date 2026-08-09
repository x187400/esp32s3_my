#ifndef __BOARD_H__
#define __BOARD_H__

#include "driver/i2c_master.h"
#include "driver/spi_common.h"

extern i2c_master_bus_handle_t i2c_bus_handle;
typedef void (*i2c_int_handler_t)(void *arg);

void Board_I2C_Init(void);
void Borad_SPI_Init(void);
void I2C_Interupt_Callback_Register(i2c_int_handler_t cb);

#endif