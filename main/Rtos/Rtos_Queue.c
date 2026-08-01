#include "Rtos_Queue.h"
#include "proj_cfg.h"

static const char *TAG = "Rtos_Queue";

/* ==================== Queue 通用实现 ==================== */

QueueHandle_t Rtos_Queue_Create(uint32_t uxQueueLength, uint32_t uxItemSize)
{
    QueueHandle_t queue = xQueueCreate(uxQueueLength, uxItemSize);
    if (queue != NULL) {
        ESP_LOGI(TAG, "Queue created OK (len=%lu, item=%lu)", uxQueueLength, uxItemSize);
    } else {
        ESP_LOGE(TAG, "Queue create FAILED (len=%lu, item=%lu)", uxQueueLength, uxItemSize);
    }
    return queue;
}

void Rtos_Queue_Delete(QueueHandle_t *queue)
{
    if (queue != NULL && *queue != NULL) {
        ESP_LOGI(TAG, "Deleting queue");
        vQueueDelete(*queue);
        *queue = NULL;
    }
}

BaseType_t Rtos_Queue_Send(QueueHandle_t queue, const void *data, TickType_t timeout)
{
    if (queue == NULL || data == NULL) {
        return pdFAIL;
    }
    return xQueueSend(queue, data, timeout);
}

BaseType_t Rtos_Queue_Receive(QueueHandle_t queue, void *data, TickType_t timeout)
{
    if (queue == NULL || data == NULL) {
        return pdFAIL;
    }
    return xQueueReceive(queue, data, timeout);
}

BaseType_t Rtos_Queue_SendFromISR(QueueHandle_t queue, const void *data, BaseType_t *pxTaskWoken)
{
    if (queue == NULL || data == NULL) {
        return pdFAIL;
    }
    return xQueueSendFromISR(queue, data, pxTaskWoken);
}

BaseType_t Rtos_Queue_ReceiveFromISR(QueueHandle_t queue, void *data, BaseType_t *pxTaskWoken)
{
    if (queue == NULL || data == NULL) {
        return pdFAIL;
    }
    return xQueueReceiveFromISR(queue, data, pxTaskWoken);
}

void Rtos_Queue_Reset(QueueHandle_t queue)
{
    if (queue != NULL) {
        xQueueReset(queue);
    }
}

uint32_t Rtos_Queue_GetPendingCount(QueueHandle_t queue)
{
    if (queue == NULL) {
        return 0;
    }
    return uxQueueMessagesWaiting(queue);
}
