#include "Rtos_msg.h"
#include "Rtos_Queue.h"
#include "Rtos_Task.h"
#include "app_led.h"
#include "app_beep.h"
#include "app_xl9555.h"
#include "app_key.h"
#include "proj_cfg.h"
#include "board.h"
#include "driver/gpio.h"
#include "sta_main.h"

static const char *TAG = "Sta_MsgTask";
volatile static bool sta_xl9555_int_flag = false;

static Sta_Main_Event_e staMainEvent = STA_IDLE_EVENT;

static void sta_i2c_int_handler(void *arg)
{
    //xl9555中断处理函数,仅配置为输入的port可进入
    (void)arg;
    sta_xl9555_int_flag = true;
}

static void sta_board_init(void)
{
    I2C_Interupt_Callback_Register(sta_i2c_int_handler);
    I2C_Init();
}

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

static void sta_init(void)
{
    #if defined(STA_XL9555_ENABLE) && (STA_XL9555_ENABLE == 1)
        App_XL9555_Init();
    #endif  

    #if defined(STA_LED_ENABLE) && (STA_LED_ENABLE == 1)
        App_Led_Init();
    #endif 

    #if defined(STA_BEEP_ENABLE) && (STA_BEEP_ENABLE == 1)
        App_Beep_Init();
    #endif 

    #if defined(STA_KEY_ENABLE) && (STA_KEY_ENABLE == 1)
        App_Key_Init();
    #endif
}

static void Sta_main(void)
{
    static bool stakeyInt = false;
    switch (staMainEvent) {
        case STA_IDLE_EVENT:
            if(sta_xl9555_int_flag)
            {
                staMainEvent = STA_XL9555_INT_EVENT;
                sta_xl9555_int_flag = false;
            }

            stakeyInt = App_Key_Int_Trigger_Check();
            if(stakeyInt == true)
            {
                staMainEvent = STA_KEY_EVENT;
            }
            
            break;

        case STA_XL9555_INT_EVENT:
            stakeyInt = App_XL9555_Key_Int_Trigger_Check();
            if(stakeyInt == true)
            {
                staMainEvent = STA_KEY_EVENT;
            }
            break;


        case STA_KEY_EVENT:
            App_Key_Scan();
            staMainEvent = STA_IDLE_EVENT;
        break;

        default:
            ESP_LOGW(TAG, "Unknown event: %d", staMainEvent);
            staMainEvent = STA_IDLE_EVENT;
            break;
    }
}

static void Sta_Task(void *pvParameters)
{
    sta_board_init();
    
    sta_init();
    while (1) {
        Sta_main();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ==================== 初始化 ==================== */

void Sta_MsgInit(void)
{
    Rtos_Task_Create(Sta_Task, "msg_task",
                     RTOS_TASK_STACK_MID, NULL,
                     RTOS_TASK_PRIO_MID, NULL);
}
