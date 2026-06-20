#ifndef __DRV_ESP8266_H
#define __DRV_ESP8266_H
#include "main.h"
#define WIFI_SSID       "YOUR_SSID"
#define WIFI_PASSWORD   "YOUR_PASSWORD"
#define MQTT_BROKER_IP  "192.168.1.100"
#define MQTT_BROKER_PORT 1883
int  esp8266_init(UART_HandleTypeDef *huart);
int  esp8266_connect_wifi(const char *ssid, const char *pwd);
int  esp8266_is_connected(void);
int  esp8266_mqtt_connect(const char *broker, uint16_t port, const char *client_id);
int  esp8266_mqtt_publish(const char *topic, const char *payload);
int  esp8266_mqtt_subscribe(const char *topic);
int  esp8266_mqtt_poll(char *topic, uint16_t tlen, char *payload, uint16_t plen);
#endif
