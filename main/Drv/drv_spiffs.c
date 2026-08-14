#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"
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
