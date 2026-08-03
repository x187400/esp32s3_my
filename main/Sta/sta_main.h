#ifndef __STA_MAIN_H__
#define __STA_MAIN_H__

typedef void (*key_event_handle)(void);

typedef enum {
    STA_IDLE_EVENT = 0,
    STA_XL9555_INT_EVENT,
    STA_KEY_EVENT,
}Sta_Main_Event_e;

void Sta_Msg_Init(void);

#endif