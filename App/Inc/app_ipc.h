#ifndef __APP_IPC_H
#define __APP_IPC_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* ======== 全局只读状态标志 ======== */
extern volatile sys_state_t  g_sys_state;
extern volatile uint32_t     g_lockout_until;
extern volatile uint32_t     g_boot_time;

/* ======== 系统事件组 ======== */
extern EventGroupHandle_t g_sys_events;

#define EVENT_UNLOCK_REQ        (1 << 0)
#define EVENT_LOCK_REQ          (1 << 1)
#define EVENT_LOCKOUT_START     (1 << 2)
#define EVENT_LOCKOUT_END       (1 << 3)
#define EVENT_WIFI_CONNECTED    (1 << 4)
#define EVENT_WIFI_LOST         (1 << 5)

/* ======== IPC 队列 ======== */
extern QueueHandle_t g_auth_queue;
extern QueueHandle_t g_log_queue;
extern QueueHandle_t g_display_queue;
extern QueueHandle_t g_cloud_queue;
extern QueueHandle_t g_cmd_queue;

/* ======== IPC 互斥信号量 ======== */
extern SemaphoreHandle_t g_flash_mutex;
extern SemaphoreHandle_t g_user_mutex;
extern SemaphoreHandle_t g_display_mutex;

#endif /* __APP_IPC_H */
