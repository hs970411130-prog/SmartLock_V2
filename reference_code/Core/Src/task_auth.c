#include "task_auth.h"
#include "task_logger.h"
#include "task_display.h"
#include "drv_relay.h"
#include "drv_buzzer.h"
#include "drv_as608.h"
#include "drv_rc522.h"
#define DEFAULT_ADMIN_PWD "88888888"

static uint8_t retry=0;
static uint32_t lockout_until=0;
static user_record_t users[MAX_USER_COUNT];
static uint8_t ucount=0;

int auth_submit_request(auth_request_t *r){ if(!r)return -1; return (xQueueSend(g_auth_queue,r,0)==pdPASS)?0:-1; }
uint8_t auth_get_retry_count(void){ return retry; }

int auth_admin_login(const char *pw){
    if(!pw)return -1;
    if(strncmp(pw,DEFAULT_ADMIN_PWD,ADMIN_PASSWORD_LENGTH)==0){ system_set_state(STATE_ADMIN_MODE); display_show_toast("admin mode"); return 0; }
    return -1;
}

int auth_unlock_door(auth_method_t m,uint32_t uid){ relay_unlock(5000); buzzer_beep(100); system_set_state(STATE_UNLOCKED); logger_add(LOG_LOCK_OPEN,uid,m,"local unlock"); return 0; }
int auth_lock_door(void){ relay_unlock(0); system_set_state(STATE_IDLE); logger_add(LOG_LOCK_CLOSE,0,AUTH_METHOD_NONE,"auto lock"); return 0; }

static int vrfy_pw(const char *pw,user_record_t *u){ return(strncmp(pw,u->password_hash,USER_PASSWORD_LENGTH)==0); }
static int vrfy_rfid(uint8_t *uid,user_record_t *u){ return(memcmp(uid,u->rfid_uid,4)==0); }
static int vrfy_fp(uint16_t id,user_record_t *u){ for(uint8_t i=0;i<u->fp_count;i++)if(u->fingerprint_ids[i]==id)return 1; return 0; }

static auth_result_t process(auth_request_t *req){
    user_record_t *mu=NULL;
    switch(req->method){
        case AUTH_METHOD_FINGERPRINT: { for(uint8_t i=0;i<ucount;i++)if(vrfy_fp(req->credential.fingerprint_id,&users[i])){ mu=&users[i]; break; } if(!mu)return AUTH_FAIL_NOT_FOUND; break; }
        case AUTH_METHOD_RFID: { for(uint8_t i=0;i<ucount;i++)if(vrfy_rfid(req->credential.rfid_uid,&users[i])){ mu=&users[i]; break; } if(!mu)return AUTH_FAIL_WRONG; break; }
        case AUTH_METHOD_PASSWORD: { for(uint8_t i=0;i<ucount;i++)if(vrfy_pw(req->credential.password,&users[i])){ mu=&users[i]; break; } if(!mu)return AUTH_FAIL_WRONG; break; }
        case AUTH_METHOD_ADMIN: return(auth_admin_login(req->credential.password)==0)?AUTH_SUCCESS:AUTH_FAIL_WRONG;
        default: return AUTH_FAIL_WRONG;
    }
    if(mu&&!mu->enabled)return AUTH_FAIL_WRONG;
    if(mu&&mu->role==ROLE_GUEST&&mu->expire_time&&get_timestamp()>mu->expire_time)return AUTH_FAIL_WRONG;
    if(mu){ auth_unlock_door(req->method,mu->id); return AUTH_SUCCESS; }
    return AUTH_FAIL_WRONG;
}

static void poll_fp(void){
    if(as608_detect_finger()){ if(!as608_capture_image(1)&&!as608_capture_image(2)&&!as608_create_template()){ uint16_t id=0,sc=0; if(!as608_search_fingerprint(&id,&sc)){ auth_request_t r; r.method=AUTH_METHOD_FINGERPRINT; r.credential.fingerprint_id=id; r.timestamp_ms=HAL_GetTick(); auth_result_t rs=process(&r); retry=(rs==AUTH_SUCCESS)?0:retry+1; display_show_auth_result(rs,AUTH_METHOD_FINGERPRINT,""); } } }
}
static void poll_rfid(void){
    uint8_t uid[4],ul=0; if(!rc522_check_card(uid,&ul)){ auth_request_t r; r.method=AUTH_METHOD_RFID; memcpy(r.credential.rfid_uid,uid,4); r.timestamp_ms=HAL_GetTick(); auth_result_t rs=process(&r); retry=(rs==AUTH_SUCCESS)?0:retry+1; display_show_auth_result(rs,AUTH_METHOD_RFID,""); logger_add(rs==AUTH_SUCCESS?LOG_LOCK_OPEN:LOG_AUTH_FAIL,0,AUTH_METHOD_RFID,""); rc522_halt(); vTaskDelay(pdMS_TO_TICKS(1000)); }
}

void task_auth_entry(void *pv){
    (void)pv;
    auth_request_t r;
    uint32_t lw=0, lfp=0, lrfid=0;
    for(;;){
        vTaskDelayUntil(&lw,pdMS_TO_TICKS(50));
        if(g_sys_state==STATE_LOCKED_OUT){ if(HAL_GetTick()>=lockout_until){ system_set_state(STATE_IDLE); retry=0; buzzer_stop(); display_show_home(); } vTaskDelay(pdMS_TO_TICKS(500)); continue; }
        if(g_sys_state==STATE_UNLOCKED&&!relay_is_unlocked())auth_lock_door();
        if(xQueueReceive(g_auth_queue,&r,0)==pdPASS){ auth_result_t rs=process(&r); if(rs==AUTH_SUCCESS)retry=0; else{ retry++; if(retry>=MAX_RETRY_ATTEMPTS){ system_set_state(STATE_LOCKED_OUT); lockout_until=HAL_GetTick()+LOCKOUT_DURATION_MS; buzzer_alarm(); logger_add(LOG_SYSTEM_LOCKOUT,0,AUTH_METHOD_NONE,"lockout"); display_show_countdown("locked",LOCKOUT_DURATION_MS); continue; } } display_show_auth_result(rs,r.method,""); logger_add(rs==AUTH_SUCCESS?LOG_LOCK_OPEN:LOG_AUTH_FAIL,0,r.method,""); }
        if(HAL_GetTick()-lfp>500){ poll_fp(); lfp=HAL_GetTick(); }
        if(HAL_GetTick()-lrfid>200){ poll_rfid(); lrfid=HAL_GetTick(); }
    }
}

int auth_register_user(user_record_t *u){ if(!u)return -1; if(ucount>=MAX_USER_COUNT)return -1; xSemaphoreTake(g_user_mutex,portMAX_DELAY); memcpy(&users[ucount],u,sizeof(user_record_t)); ucount++; xSemaphoreGive(g_user_mutex); return 0; }
int auth_delete_user(uint32_t id){ xSemaphoreTake(g_user_mutex,portMAX_DELAY); for(uint8_t i=0;i<ucount;i++){ if(users[i].id==id){ if(i<ucount-1)memmove(&users[i],&users[i+1],(ucount-i-1)*sizeof(user_record_t)); ucount--; break; } } xSemaphoreGive(g_user_mutex); return 0; }
