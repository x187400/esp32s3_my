#ifndef __DRV_TOUCH_H__
#define __DRV_TOUCH_H__

/* ---- CHSC5432 寄存器（应用笔记）---- */
#define CHSC_TP_INFO_IC_TYPE   0x20000080u   /* ictype: 0x05 = CHSC5432 */
#define CHSC_TP_INFO_LCD_X     0x20000086u   /* lcdX 分辨率(16bit) */
#define CHSC_TP_INFO_LCD_Y     0x20000088u   /* lcdY 分辨率(16bit) */
#define CHSC_TOUCH_DATA_REG    0x2000002Cu   /* 触摸数据起始 */
#define CHSC_TOUCH_DATA_LEN    28u           /* 默认读取 28 字节 */
#define CHSC_TOUCH_MAX_POINTS  10u
#define CHSC_EVENT_NORMAL      0xFFu         /* 正常触摸事件 */
#define CHSC_EVENT_GESTURE     0xFEu         /* 手势事件 */

#include "esp_lcd_touch.h"

void Drv_Touch_Init(void);
esp_err_t Drv_Touch_Create(esp_lcd_touch_handle_t *tp);

#endif