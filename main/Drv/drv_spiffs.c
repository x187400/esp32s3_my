#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_heap_caps.h"
#include "lvgl.h"
#include "drv_spiffs.h"

static const char *TAG = "Drv_Spiffs";

#define SPIFFS_MOUNT_POINT       "/spiffs"
#define SPIFFS_PARTITION_LABEL   "storage"

esp_err_t Drv_Spiffs_Init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path              = SPIFFS_MOUNT_POINT,
        .partition_label        = SPIFFS_PARTITION_LABEL,
        .max_files              = 5,
        .format_if_mount_failed = true,
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS mount failed (%s)", esp_err_to_name(ret));
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(SPIFFS_PARTITION_LABEL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS partition info failed (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "SPIFFS mounted at %s: total=%u KB, used=%u KB",
                 SPIFFS_MOUNT_POINT, (unsigned)(total / 1024), (unsigned)(used / 1024));
    }
    return ESP_OK;
}

/* 从 SPIFFS 读取图片文件到内存，构造 LVGL 图像描述符（decoder 会自动识别 PNG magic） */
bool Drv_Spiffs_Load_Image(const char *path, lv_image_dsc_t *dsc)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "open %s failed", path);
        return false;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) {
        fclose(f);
        return false;
    }

    uint8_t *buf = heap_caps_malloc(sz, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!buf) {
        buf = malloc(sz);
    }
    if (!buf) {
        fclose(f);
        return false;
    }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) {
        heap_caps_free(buf);
        return false;
    }

    memset(dsc, 0, sizeof(*dsc));
    dsc->header.magic = LV_IMAGE_HEADER_MAGIC; /* 标记为"变量图像"，LVGL 才会走 decoder 解析 */
    dsc->header.cf = LV_COLOR_FORMAT_ARGB8888; /* 具体格式由解码器解析 */
    dsc->data = buf;
    dsc->data_size = (uint32_t)sz;
    return true;
}
