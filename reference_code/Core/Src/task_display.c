#include "task_display.h"
#include "drv_oled.h"
static display_page_t pg=PAGE_HOME;
static char toast[32];
static uint32_t te=0;

static void render_home(void){
    oled_clear(); oled_show_string(0,0,"SmartLock v1.0",1);
    const char *s="idle";
    switch(g_sys_state){ case STATE_WAITING_AUTH:s="waiting";break; case STATE_UNLOCKED:s="unlocked";break; case STATE_LOCKED_OUT:s="locked";break; case STATE_ADMIN_MODE:s="admin";break; default:break; }
    oled_show_string(0,12,s,1); oled_draw_lock_icon(110,10,g_sys_state!=STATE_UNLOCKED);
    RTC_TimeTypeDef t; RTC_DateTypeDef d; HAL_RTC_GetTime(&hrtc,&t,RTC_FORMAT_BIN); HAL_RTC_GetDate(&hrtc,&d,RTC_FORMAT_BIN);
    char b[16]; snprintf(b,sizeof(b),"%02d:%02d:%02d",t.Hours,t.Minutes,t.Seconds); oled_show_string(20,28,b,2);
    snprintf(b,sizeof(b),"20%02d-%02d-%02d",d.Year,d.Month,d.Date); oled_show_string(10,50,b,1);
    if(HAL_GetTick()<te&&toast[0])oled_show_string(0,56,toast,1);
}
static void render_auth(auth_result_t r,auth_method_t m,const char *u){ (void)m;(void)u; oled_clear(); if(r==AUTH_SUCCESS){ oled_show_string(30,0,"success!",1); oled_draw_lock_icon(50,15,0); if(u&&u[0])oled_show_string(0,36,u,1); oled_show_string(0,48,"unlocked",1); } else { oled_show_string(30,0,"failed!",1); const char *rs="error"; switch(r){ case AUTH_FAIL_WRONG:rs="wrong";break; case AUTH_FAIL_LOCKOUT:rs="locked";break; default:break; } oled_show_string(0,16,rs,1); uint8_t rem=MAX_RETRY_ATTEMPTS-auth_get_retry_count(); char b[16]; snprintf(b,sizeof(b),"remain:%d",rem>0?rem:0); oled_show_string(0,32,b,1); } }
static void render_cd(const char *m,uint32_t rm){ oled_clear(); oled_show_string(0,0,m,1); uint32_t s=rm/1000; char b[16]; snprintf(b,sizeof(b),"%lus",(unsigned long)s); oled_show_string(20,28,b,2); oled_draw_progress_bar(10,48,108,10,(uint32_t)(LOCKOUT_DURATION_MS-rm)*100/LOCKOUT_DURATION_MS); }

void task_display_entry(void *pv){ (void)pv; uint32_t lr=0; for(;;){ vTaskDelayUntil(&lr,pdMS_TO_TICKS(500)); switch(pg){ case PAGE_HOME:default:render_home();break; } /* oled_refresh(); */ } }
void display_set_page(display_page_t p){ pg=p; }
void display_show_auth_result(auth_result_t r,auth_method_t m,const char *u){ pg=PAGE_AUTH_RESULT; render_auth(r,m,u); if(r==AUTH_SUCCESS)te=HAL_GetTick()+3000; }
void display_show_countdown(const char *m,uint32_t rm){ render_cd(m,rm); }
void display_show_toast(const char *m){ strncpy(toast,m,31); te=HAL_GetTick()+3000; }
void display_show_home(void){ pg=PAGE_HOME; }
