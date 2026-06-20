#include "task_keypad.h"
#include "task_auth.h"
#include "drv_keypad.h"
static char pw[16]; static uint8_t pwlen=0, active=0;
static void submit(void){ if(!pwlen)return; pw[pwlen]=0; auth_request_t r; r.method=AUTH_METHOD_PASSWORD; r.timestamp_ms=HAL_GetTick(); strncpy(r.credential.password,pw,15); auth_submit_request(&r); active=0; pwlen=0; memset(pw,0,16); }
static void submit_admin(void){ if(pwlen<ADMIN_PASSWORD_LENGTH)return; auth_request_t r; r.method=AUTH_METHOD_ADMIN; r.timestamp_ms=HAL_GetTick(); strncpy(r.credential.password,pw,15); auth_submit_request(&r); active=0; pwlen=0; memset(pw,0,16); }
void task_keypad_entry(void *pv){ (void)pv; keypad_init(); uint32_t ls=0;
    for(;;){ vTaskDelayUntil(&ls,pdMS_TO_TICKS(20)); uint8_t k=keypad_scan_debounced(); if(k==KEY_NONE)continue;
        if(keypad_is_digit(k)){ if(!active){ active=1; pwlen=0; memset(pw,0,16); } if(pwlen<15)pw[pwlen++]=keypad_to_ascii(k); }
        else switch(k){ case KEY_CONFIRM:submit();break; case KEY_CANCEL:active=0;pwlen=0;break; case KEY_ADMIN:submit_admin();break; case KEY_CLEAR:if(pwlen)pwlen--; pw[pwlen]=0;break; }
    }
}
