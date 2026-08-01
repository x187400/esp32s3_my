#include "Rtos_msg.h"
#include "Rtos_Queue.h"
#include "Rtos_Task.h"
#include "app_led.h"
#include "app_xl9555.h"
#include "board.h"
#include "esp_log.h"

static const char *TAG = "Sta_MsgTask";

// /* ==================== 消息处理任务 ==================== */
// static QueueHandle_t g_msg_queue = NULL;
// static void Sta_QueueTask(void *pvParameters)
// {
//     Msg_t msg;
//     if (Rtos_Queue_Receive(g_msg_queue, &msg, portMAX_DELAY) == pdPASS) 
//     {
//         switch (msg.type) {
//         case MSG_TYPE_SENSOR:
//             ESP_LOGI(TAG, "Sensor: temp=%.1f, hum=%.1f, ts=%lu",
//                         msg.data.fvalue1, msg.data.fvalue2, msg.data.timestamp);
//             break;

//         case MSG_TYPE_CMD:
//             ESP_LOGI(TAG, "Cmd: id=%ld, param=%ld",
//                         msg.data.ivalue1, msg.data.ivalue2);
//             break;

//         case MSG_TYPE_EVENT:
//             ESP_LOGI(TAG, "Event: id=%ld, data=%lu",
//                         msg.data.ivalue1, msg.data.extra);
//             break;

//         default:
//             ESP_LOGW(TAG, "Unknown msg type: %d", msg.type);
//             break;
//         }
//     }
// }

static void Sta_Task(void *pvParameters)
{
    I2C_Init();
    App_Led_Init();
    App_XL9555_Init();
    uint8_t val = 0xff;
    App_XL9555_IO_Cfg(XL9555_DIR_OUT);
    while (1) {
        App_Led_Set(LED_ON);
        App_XL9555_Set_Val(0);
        val = App_XL9555_Get_Val();
        ESP_LOGI(TAG, "vol = %d",val);

        vTaskDelay(pdMS_TO_TICKS(500));

        App_Led_Set(LED_OFF);
        App_XL9555_Set_Val(1);
        val = App_XL9555_Get_Val();
        ESP_LOGI(TAG, "vol = %d",val);
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* ==================== 初始化 ==================== */

void Sta_MsgInit(void)
{
    Rtos_Task_Create(Sta_Task, "msg_task",
                     RTOS_TASK_STACK_MID, NULL,
                     RTOS_TASK_PRIO_MID, NULL);
}
