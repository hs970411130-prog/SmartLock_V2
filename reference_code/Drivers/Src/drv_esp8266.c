#include "drv_esp8266.h"
static UART_HandleTypeDef *esp_u=NULL;
static uint8_t wifi_ok=0;
static char rx_buf[512];
static volatile uint16_t rx_i=0;
static volatile uint8_t rx_done=0;
int esp8266_send_at(const char *cmd, const char *expect, uint32_t to) {
    rx_done=0; rx_i=0;
    HAL_UART_Transmit(esp_u,(uint8_t*)cmd,strlen(cmd),100);
    HAL_UART_Transmit(esp_u,(uint8_t*)"
",2,100);
    uint32_t dl=HAL_GetTick()+to;
    while(HAL_GetTick()<dl){ if(rx_done){ rx_done=0; if(strstr(rx_buf,expect))return 0; if(strstr(rx_buf,"ERROR"))return -2; } }
    return -1;
}
int esp8266_init(UART_HandleTypeDef *h){ esp_u=h; rx_i=0; rx_done=0; if(esp8266_send_at("AT","OK",2000))return -1; esp8266_send_at("AT+CWMODE=1","OK",1000); wifi_ok=0; return 0; }
int esp8266_connect_wifi(const char *ssid, const char *pwd){ char c[128]; snprintf(c,sizeof(c),"AT+CWJAP=\"%s\",\"%s\"",ssid,pwd); int r=esp8266_send_at(c,"WIFI GOT IP",15000); if(!r)wifi_ok=1; return r; }
int esp8266_is_connected(void){ return wifi_ok; }
int esp8266_mqtt_connect(const char *b,uint16_t p,const char *id){ char c[256]; snprintf(c,sizeof(c),"AT+MQTTUSERCFG=0,1,\"%s\",\"\",\"\",0,0,\"\"",id); esp8266_send_at(c,"OK",2000); snprintf(c,sizeof(c),"AT+MQTTCONN=0,\"%s\",%d,0",b,p); return esp8266_send_at(c,"OK",10000); }
int esp8266_mqtt_publish(const char *t,const char *pl){ char c[384]; snprintf(c,sizeof(c),"AT+MQTTPUB=0,\"%s\",\"%s\",0,0",t,pl); return esp8266_send_at(c,"OK",5000); }
int esp8266_mqtt_subscribe(const char *t){ char c[128]; snprintf(c,sizeof(c),"AT+MQTTSUB=0,\"%s\",0",t); return esp8266_send_at(c,"OK",3000); }
int esp8266_mqtt_poll(char *t,uint16_t tl,char *pl,uint16_t plen){ if(rx_done&&strstr(rx_buf,"+MQTTSUBRECV:")){ rx_done=0; if(t)snprintf(t,tl,"cmd"); if(pl)snprintf(pl,plen,"%s",rx_buf); return 1; } return 0; }
