#include "hal_spi.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "proj_cfg.h"
#include "drv_sdCard.h"
#include <stddef.h>

static const char *TAG = "Drv_SdCard";

esp_err_t Drv_SD_Card_Init(void)
{
    esp_err_t ret = ESP_OK;
    sdmmc_card_t *card;
    const char mount_point[] = MOUNT_POINT;

    Hal_Spi_Add_Dev(SPI_TF_CS_PIN);

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t dev_cfg = {
        .host_id = host.slot,
        .gpio_cs = SPI_TF_CS_PIN,
        .gpio_cd = SDSPI_SLOT_NO_CD,
        .gpio_wp = SDSPI_SLOT_NO_WP,
        .gpio_int = SDSPI_SLOT_NO_INT,
    };
    ret = esp_vfs_fat_sdspi_mount(mount_point,&host,&dev_cfg,&mount_config,&card);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG,"mount sys faild:%x,remove sdCard",ret);
        Hal_Spi_Remove_Device();
        return ret;
    }
    return ret;
}

void Drv_Get_SdCard_Usage(size_t *out_total_bytes, size_t *out_free_bytes)
{
    FATFS *fs;
    DWORD free_clusters;
    int res = f_getfree("0:", &free_clusters, &fs);
    assert(res == FR_OK);
    size_t total_sectors = (fs->n_fatent - 2) * fs->csize;
    size_t free_sectors = free_clusters * fs->csize;

    size_t sd_total = total_sectors/1024;
    size_t sd_total_KB = sd_total * fs->ssize;   
    size_t sd_free = free_sectors / 1024;

    size_t sd_free_KB = sd_free * fs->ssize;

    if(out_total_bytes != NULL)
    {
        *out_total_bytes = sd_total_KB;
    }

    if(out_free_bytes != NULL)
    {
        *out_free_bytes = sd_free_KB;
    }
}