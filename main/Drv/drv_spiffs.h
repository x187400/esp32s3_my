#ifndef __DRV_SPIFFS_H__
#define __DRV_SPIFFS_H__

#include "esp_err.h"
#include "lvgl.h"


/* SPIFFS 图片路径（spiffs/ 目录随固件烧录） */
#define IMG_PATH_MUSIC      "/spiffs/app_music.png"
#define IMG_PATH_COMPONENT1 "/spiffs/app_music1_component.png"
#define IMG_PATH_COMPONENT2 "/spiffs/app_music2_component.png"

/* 初始化并挂载 SPIFFS（分区 label: storage，挂载点 /spiffs） */
esp_err_t Drv_Spiffs_Init(void);

/* 从 SPIFFS 读取图片文件到内存并构造 LVGL 图像描述符（decoder 会自动识别 PNG magic）
 * 注意：返回 true 后，dsc->data 指向 malloc 的内存，需在图片生命周期内保持有效 */
bool Drv_Spiffs_Load_Image(const char *path, lv_image_dsc_t *dsc);

#endif
