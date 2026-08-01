#ifndef __MSG_H__
#define __MSG_H__

#include <stdint.h>

/* ==================== 消息类型枚举 ==================== */

typedef enum {
    MSG_TYPE_NONE = 0,
    MSG_TYPE_SENSOR,        // 传感器数据
    MSG_TYPE_CMD,           // 控制命令
    MSG_TYPE_EVENT,         // 事件通知
    MSG_TYPE_COUNT
} MsgType_t;

/* ==================== 统一的数据承载结构体 ==================== */

typedef struct {
    int32_t  ivalue1;       // 通用整型字段 1
    int32_t  ivalue2;       // 通用整型字段 2
    float    fvalue1;       // 通用浮点字段 1
    float    fvalue2;       // 通用浮点字段 2
    uint32_t timestamp;     // 时间戳
    uint32_t extra;         // 扩展字段
} MsgData_t;

/* ==================== 消息队列元素 ==================== */

typedef struct {
    MsgType_t  type;        // 消息类型（决定 data 如何解读）
    MsgData_t  data;        // 统一数据体
} Msg_t;

#endif
