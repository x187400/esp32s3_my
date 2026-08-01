#include "Rtos_Task.h"
#include "proj_cfg.h"

static const char *TAG = "Rtos_Task";

/* ==================== Task 通用实现 ==================== */

void Rtos_Task_Create(TaskFunction_t pxTaskCode,
                      const char *pcName,
                      uint32_t ulStackDepth,
                      void *pvParameters,
                      UBaseType_t uxPriority,
                      TaskHandle_t *pxCreatedTask)
{
    BaseType_t ret = xTaskCreate(pxTaskCode, pcName,
                                 ulStackDepth, pvParameters,
                                 uxPriority, pxCreatedTask);
    if (ret == pdPASS) {
        ESP_LOGI(TAG, "Task [%s] created OK", pcName);
    } else {
        ESP_LOGE(TAG, "Task [%s] create FAILED", pcName);
    }
}

void Rtos_Task_Delete(TaskHandle_t *handle)
{
    if (handle != NULL && *handle != NULL) {
        ESP_LOGI(TAG, "Deleting task");
        vTaskDelete(*handle);
        *handle = NULL;
    }
}

void Rtos_Task_Suspend(TaskHandle_t handle)
{
    if (handle != NULL) {
        vTaskSuspend(handle);
    }
}

void Rtos_Task_Resume(TaskHandle_t handle)
{
    if (handle != NULL) {
        vTaskResume(handle);
    }
}

uint32_t Rtos_Task_GetFreeStack(void)
{
    return uxTaskGetStackHighWaterMark(NULL);
}