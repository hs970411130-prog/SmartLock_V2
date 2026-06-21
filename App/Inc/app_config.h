#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

#include <stdint.h>

/* ======== 编译开关 ======== */
#define ENABLE_FINGERPRINT      1
#define ENABLE_RFID             1
#define ENABLE_KEYPAD           1
#define ENABLE_WIFI             0   // 当前关闭

/* ======== 日志级别 ======== */
// #define LOG_DEBUG_EN              // 上线版本强制关闭

/* ======== 系统容量 ======== */
#define MAX_USER_COUNT          100
#define MAX_FINGERPRINT_COUNT   50
#define MAX_LOG_ENTRIES         500
#define MAX_FP_PER_USER         5

/* ======== 认证与安全 ======== */
#define MAX_RETRY_ATTEMPTS      5
#define LOCKOUT_DURATION_MS     30000
#define UNLOCK_DURATION_MS      5000
#define PASSWORD_LEN            6
#define ADMIN_PASSWORD_LEN      8
#define ADMIN_FIXED_PASSWORD    "88888888"

/* ======== Flash 分区地址（W25Q64） ======== */
#define FLASH_LOG_ADDR          0x000000
#define FLASH_FP_ADDR           0x040000
#define FLASH_USER_ADDR         0x080000

/* ======== FreeRTOS 任务优先级 ======== */
#define PRIO_AUTH               5
#define PRIO_CLOUD              4
#define PRIO_LOGGER             3
#define PRIO_DISPLAY            2
#define PRIO_KEYPAD             1
#define PRIO_SUPERVISOR         0

/* ======== FreeRTOS 任务栈（含 20% 冗余） ======== */
#define STACK_AUTH              2048
#define STACK_CLOUD             1024
#define STACK_LOGGER            1024
#define STACK_DISPLAY           512
#define STACK_KEYPAD            512
#define STACK_SUPERVISOR        256

/* ======== IPC 队列深度 ======== */
#define QUEUE_AUTH_DEPTH        10
#define QUEUE_LOG_DEPTH         20
#define QUEUE_DISPLAY_DEPTH     8
#define QUEUE_CLOUD_DEPTH       10
#define QUEUE_CMD_DEPTH         5

/* ======== OLED ======== */
#define OLED_WIDTH              128
#define OLED_HEIGHT             64

#endif /* __APP_CONFIG_H */
