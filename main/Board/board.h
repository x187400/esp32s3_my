#ifndef __BOARD_H__
#define __BOARD_H__

#include "driver/i2c_master.h"

extern i2c_master_bus_handle_t i2c_bus_handle;
void I2C_Init(void);
#endif