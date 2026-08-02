#include "Rtos_Queue.h"
#include "Rtos_msg.h"
#include "proj_cfg.h"

static const char *TAG = "Rtos_Queue";
static QueueHandle_t queue = NULL;
/* ==================== Queue 通用实现 ==================== */
BaseType_t Rtos_Queue_Create(uint32_t uxQueueLength, uint32_t uxItemSize)
{
    BaseType_t ret = pdPASS;
    queue = xQueueCreate(uxQueueLength, uxItemSize);
    if (queue != NULL) {
        ESP_LOGI(TAG, "Queue created OK (len=%lu, item=%lu)", uxQueueLength, uxItemSize);
    } else {
        ESP_LOGE(TAG, "Queue create FAILED (len=%lu, item=%lu)", uxQueueLength, uxItemSize);
        ret = pdFAIL;
    }
    return ret;
}

BaseType_t Rtos_Queue_Send(Msg_t *msg, TickType_t timeout)
{
    if (queue == NULL || msg == NULL) {
        return pdFAIL;
    }
    return xQueueSend(queue, msg, timeout);
}

BaseType_t Rtos_Queue_Receive(Msg_t *msg ,TickType_t timeout)
{
    if (queue == NULL || msg == NULL) {
        return pdFAIL;
    }
    return xQueueReceive(queue, msg, timeout);
}

BaseType_t Rtos_Queue_SendFromISR(Msg_t *msg, BaseType_t *pxTaskWoken)
{
    if (queue == NULL || msg == NULL) {
        return pdFAIL;
    }
    return xQueueSendFromISR(queue, msg, pxTaskWoken);
}

