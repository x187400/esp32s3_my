#ifndef __RTOS_TASK_H__
#define __RTOS_TASK_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ==================== 默认配置宏 ==================== */

#define RTOS_TASK_PRIO_LOW       1
#define RTOS_TASK_PRIO_MID       3
#define RTOS_TASK_PRIO_HIGH      5

#define RTOS_TASK_STACK_SMALL    2048
#define RTOS_TASK_STACK_MID      4096
#define RTOS_TASK_STACK_LARGE    8192

/* ==================== Task 通用接口 ==================== */

/**
 * @brief  通用的 Task 创建封装（带日志）
 * @param  pxTaskCode    任务函数指针
 * @param  pcName        任务名称（用于调试）
 * @param  ulStackDepth  栈大小（字节）
 * @param  pvParameters  传入参数
 * @param  uxPriority    优先级
 * @param  pxCreatedTask 返回的任务句柄（可为 NULL）
 */
void Rtos_Task_Create(TaskFunction_t pxTaskCode,
                      const char *pcName,
                      uint32_t ulStackDepth,
                      void *pvParameters,
                      UBaseType_t uxPriority,
                      TaskHandle_t *pxCreatedTask);

/**
 * @brief  删除一个任务
 * @param  handle  任务句柄指针的地址（删除后自动置 NULL）
 */
void Rtos_Task_Delete(TaskHandle_t *handle);

/**
 * @brief  挂起一个任务
 * @param  handle  任务句柄
 */
void Rtos_Task_Suspend(TaskHandle_t handle);

/**
 * @brief  恢复一个挂起的任务
 * @param  handle  任务句柄
 */
void Rtos_Task_Resume(TaskHandle_t handle);

/**
 * @brief  获取当前任务的剩余栈空间（字节）
 * @return 剩余栈大小
 */
uint32_t Rtos_Task_GetFreeStack(void);

#endif