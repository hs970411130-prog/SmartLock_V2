#include "task_cloud.h"
#include "task_logger.h"
#include "drv_esp8266.h"
void task_cloud_entry(void *pv){ (void)pv;
#if ENABLE_WIFI
    esp8266_init(&huart3);
    if(!esp8266_connect_wifi(WIFI_SSID,WIFI_PASSWORD)){ xEventGroupSetBits(g_sys_events,EVENT_WIFI_CONNECTED); logger_add(LOG_WIFI_CONNECT,0,AUTH_METHOD_NONE,WIFI_SSID); esp8266_mqtt_connect(MQTT_BROKER_IP,MQTT_BROKER_PORT,"smartlock_001"); esp8266_mqtt_subscribe("smartlock/cmd"); }
#endif
    for(;;){
#if ENABLE_WIFI
        if(!esp8266_is_connected()){ vTaskDelay(pdMS_TO_TICKS(5000)); esp8266_connect_wifi(WIFI_SSID,WIFI_PASSWORD); continue; }
        char t[64],pl[256]; if(esp8266_mqtt_poll(t,sizeof(t),pl,sizeof(pl))>0){ if(strstr(pl,"unlock")){ auth_request_t r; r.method=AUTH_METHOD_OTP; r.timestamp_ms=HAL_GetTick(); auth_submit_request(&r); } }
        static uint32_t lh=0; if(HAL_GetTick()-lh>30000){ char h[64]; snprintf(h,sizeof(h),"{\"state\":%d,\"uptime\":%lu,\"logs\":%lu}",g_sys_state,(unsigned long)supervisor_get_uptime(),(unsigned long)logger_get_count()); esp8266_mqtt_publish("smartlock/status",h); lh=HAL_GetTick(); }
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
int cloud_report_unlock(uint32_t uid,auth_method_t m){
#if ENABLE_WIFI
    if(!esp8266_is_connected())return-1; char p[128]; snprintf(p,sizeof(p),"{\"event\":\"unlock\",\"user\":%lu,\"method\":%d}",(unsigned long)uid,m); return esp8266_mqtt_publish("smartlock/log",p);
#else
    (void)uid;(void)m; return -1;
#endif
}
int cloud_report_alarm(const char *r){
#if ENABLE_WIFI
    if(!esp8266_is_connected())return-1; char p[128]; snprintf(p,sizeof(p),"{\"event\":\"alarm\",\"reason\":\"%s\"}",r); return esp8266_mqtt_publish("smartlock/status",p);
#else
    (void)r; return -1;
#endif
}
