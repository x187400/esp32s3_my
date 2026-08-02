#ifndef __RTOS_QUEUE_H__
#define __RTOS_QUEUE_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Rtos_msg.h"
/* ==================== 默认队列长度宏 ==================== */

#define RTOS_QUEUE_LEN_SMALL     10
#define RTOS_QUEUE_LEN_MID       20
#define RTOS_QUEUE_LEN_LARGE     50

/* ==================== Queue 通用接口 ==================== */

/**
 * @brief  创建系统唯一消息队列（带日志，单例模式）
 * @param  uxQueueLength  队列最大元素数
 * @param  uxItemSize     每个元素的大小（字节），通常为 sizeof(Msg_t)
 * @return pdPASS 创建成功 / pdFAIL 创建失败
 * @note   单例队列：模块内部持有句柄，Send/Receive 无需再传句柄参数
 */
BaseType_t Rtos_Queue_Create(uint32_t uxQueueLength, uint32_t uxItemSize);

/**
 * @brief  向消息队列发送一条消息（阻塞）
 * @param  msg     待发送的消息指针（内容按值拷贝入队）
 * @param  timeout 超时时间（ticks），portMAX_DELAY 为永久等待，0 为非阻塞
 * @return pdPASS / pdFAIL
 */
BaseType_t Rtos_Queue_Send(Msg_t *msg, TickType_t timeout);

/**
 * @brief  从消息队列接收一条消息（阻塞）
 * @param  msg     接收缓冲区指针，用于存放接收到的消息
 * @param  timeout 超时时间（ticks），portMAX_DELAY 为永久等待，0 为非阻塞
 * @return pdPASS / pdFAIL
 */
BaseType_t Rtos_Queue_Receive(Msg_t *msg, TickType_t timeout);

/**
 * @brief  向消息队列发送一条消息（ISR 中调用）
 * @param  msg         待发送的消息指针（内容按值拷贝入队）
 * @param  pxTaskWoken 高优先级任务被唤醒标志，可在退出 ISR 前检查并切换上下文
 * @return pdPASS / pdFAIL
 */
BaseType_t Rtos_Queue_SendFromISR(Msg_t *msg, BaseType_t *pxTaskWoken);

#endif
