#ifndef __STA_UI_H__
#define __STA_UI_H__

#include "lvgl.h"

#define UI_HOR_RES   240
#define UI_VER_RES   320
#define HOME_PAGE_NUM  2

typedef struct
{
    lv_obj_t * homePage[HOME_PAGE_NUM];
}Sta_Ui_t;


/* 横屏双页 UI：一级页(标题+图片+指示条) / 二级页(两张组件图)，滑动切换 */
void Sta_Ui_Show(void);
void Sta_Ui_HardWare_Init(void);
#endif