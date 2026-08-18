#ifndef __STA_UI_HOME_H__
#define __STA_UI_HOME_H__

#include "lvgl.h"

#define UI_HOR_RES      240
#define UI_VER_RES      320
#define HOME_PAGE_NUM   1

#define UI_STRUCT_H         30
#define UI_HOME_NAVBAR_H    30
#define UI_HOME_H           UI_VER_RES - UI_STRUCT_H - UI_HOME_NAVBAR_H

#define UI_NAVBAR_DOT_NUM   3   /* 页面指示栏圆点数量 */

/* 页面内容创建函数类型：每页一个函数，负责向 page 内填充该页控件 */
typedef void (*page_create_fn_t)(lv_obj_t *page);

typedef enum
{
    HOME_MUSIC,
    HOME_MAX,
}Sta_Ui_Home_App_e;

typedef struct
{
    lv_obj_t * homePage[HOME_PAGE_NUM];
    lv_obj_t * navDot[UI_NAVBAR_DOT_NUM];   /* 页面指示圆点（navDot[0]=最左） */
    lv_obj_t * basePage;
}Sta_Ui_t;


/* 横屏双页 UI：一级页(标题+图片+指示条) / 二级页(两张组件图)，滑动切换 */
void Sta_Ui_Show(void);
void Sta_Ui_HardWare_Init(void);
#endif