#ifndef __RTOS_QUEUE_H__
#define __RTOS_QUEUE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/* ==================== 默认队列长度宏 ==================== */

#define RTOS_QUEUE_LEN_SMALL     10
#define RTOS_QUEUE_LEN_MID       20
#define RTOS_QUEUE_LEN_LARGE     50

/* ==================== Queue 通用接口 ==================== */

/**
 * @brief  创建一个消息队列（带日志）
 * @param  uxQueueLength  队列最大元素数
 * @param  uxItemSize     每个元素的大小（字节）
 * @return 队列句柄，失败返回 NULL
 */
QueueHandle_t Rtos_Queue_Create(uint32_t uxQueueLength, uint32_t uxItemSize);

/**
 * @brief  删除一个队列
 * @param  queue  队列句柄指针的地址（删除后自动置 NULL）
 */
void Rtos_Queue_Delete(QueueHandle_t *queue);

/**
 * @brief  向队列发送数据（阻塞）
 * @param  queue   队列句柄
 * @param  data    数据指针
 * @param  timeout 超时时间（ticks），portMAX_DELAY 为永久等待
 * @return pdPASS / pdFAIL
 */
BaseType_t Rtos_Queue_Send(QueueHandle_t queue, const void *data, TickType_t timeout);

/**
 * @brief  从队列接收数据（阻塞）
 * @param  queue   队列句柄
 * @param  data    接收缓冲区指针
 * @param  timeout 超时时间（ticks），portMAX_DELAY 为永久等待
 * @return pdPASS / pdFAIL
 */
BaseType_t Rtos_Queue_Receive(QueueHandle_t queue, void *data, TickType_t timeout);

/**
 * @brief  向队列发送数据（ISR 中调用）
 * @param  queue        队列句柄
 * @param  data         数据指针
 * @param  pxTaskWoken  上下文切换标志
 * @return pdPASS / pdFAIL
 */
BaseType_t Rtos_Queue_SendFromISR(QueueHandle_t queue, const void *data, BaseType_t *pxTaskWoken);

/**
 * @brief  从队列接收数据（ISR 中调用）
 * @param  queue        队列句柄
 * @param  data         接收缓冲区指针
 * @param  pxTaskWoken  上下文切换标志
 * @return pdPASS / pdFAIL
 */
BaseType_t Rtos_Queue_ReceiveFromISR(QueueHandle_t queue, void *data, BaseType_t *pxTaskWoken);

/**
 * @brief  重置队列（清空所有未读消息）
 * @param  queue  队列句柄
 */
void Rtos_Queue_Reset(QueueHandle_t queue);

/**
 * @brief  获取队列中等待的消息数量
 * @param  queue  队列句柄
 * @return 等待的消息数量
 */
uint32_t Rtos_Queue_GetPendingCount(QueueHandle_t queue);

#endif
