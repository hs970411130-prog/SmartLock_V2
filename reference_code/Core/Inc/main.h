#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

/* ======== System Config ======== */
#define ENABLE_FINGERPRINT      1
#define ENABLE_RFID             1
#define ENABLE_KEYPAD           1
#define ENABLE_WIFI             0

#define MAX_RETRY_ATTEMPTS      5
#define LOCKOUT_DURATION_MS     30000
#define ADMIN_PASSWORD_LENGTH   8
#define USER_PASSWORD_LENGTH    6
#define MAX_FINGERPRINT_COUNT   50
#define MAX_USER_COUNT          100
#define MAX_LOG_ENTRIES         500
#define FLASH_LOG_ADDR          0x00000
#define FLASH_FP_ADDR           0x40000
#define FLASH_USER_ADDR         0x80000

#define PRIO_AUTH               5
#define PRIO_CLOUD              4
#define PRIO_LOGGER             3
#define PRIO_DISPLAY            2
#define PRIO_KEYPAD             1
#define PRIO_SUPERVISOR         0

#define STACK_AUTH              2048
#define STACK_CLOUD             1024
#define STACK_LOGGER            1024
#define STACK_DISPLAY           512
#define STACK_KEYPAD            512
#define STACK_SUPERVISOR        256

#define QUEUE_AUTH_DEPTH        10
#define QUEUE_LOG_DEPTH         20
#define OLED_WIDTH              128
#define OLED_HEIGHT             64

/* ======== Types ======== */
typedef enum {
    AUTH_METHOD_NONE = 0,
    AUTH_METHOD_FINGERPRINT = 1,
    AUTH_METHOD_RFID = 2,
    AUTH_METHOD_PASSWORD = 3,
    AUTH_METHOD_OTP = 4,
    AUTH_METHOD_ADMIN = 5,
} auth_method_t;

typedef struct {
    auth_method_t method;
    union {
        uint16_t fingerprint_id;
        uint8_t  rfid_uid[4];
        char     password[16];
        char     otp_code[8];
    } credential;
    uint32_t timestamp_ms;
} auth_request_t;

typedef enum {
    AUTH_SUCCESS = 0,
    AUTH_FAIL_WRONG = 1,
    AUTH_FAIL_LOCKOUT = 2,
    AUTH_FAIL_NOT_FOUND = 3,
    AUTH_FAIL_TIMEOUT = 4,
} auth_result_t;

typedef enum {
    ROLE_GUEST = 0,
    ROLE_USER = 1,
    ROLE_ADMIN = 2,
} user_role_t;

typedef struct {
    uint32_t    id;
    char        name[16];
    uint8_t     rfid_uid[4];
    char        password_hash[32];
    uint8_t     fingerprint_ids[5];
    uint8_t     fp_count;
    user_role_t role;
    uint32_t    created_time;
    uint32_t    expire_time;
    uint8_t     enabled;
} user_record_t;

typedef enum {
    LOG_LOCK_OPEN = 0,
    LOG_LOCK_CLOSE = 1,
    LOG_AUTH_FAIL = 2,
    LOG_SYSTEM_LOCKOUT = 3,
    LOG_ADMIN_CONFIG = 4,
    LOG_WIFI_CONNECT = 5,
    LOG_WIFI_REMOTE = 6,
    LOG_SYSTEM_BOOT = 7,
} log_type_t;

typedef struct {
    log_type_t  type;
    uint32_t    timestamp;
    uint32_t    user_id;
    auth_method_t method;
    char        detail[32];
} log_entry_t;

typedef enum {
    STATE_IDLE,
    STATE_WAITING_AUTH,
    STATE_AUTHENTICATING,
    STATE_UNLOCKED,
    STATE_LOCKED_OUT,
    STATE_ADMIN_MODE,
    STATE_ERROR,
} system_state_t;

typedef enum {
    PAGE_HOME,
    PAGE_AUTH_RESULT,
    PAGE_SETTINGS,
    PAGE_LOG_VIEW,
} display_page_t;

/* ======== Global Variables ======== */
extern system_state_t   g_sys_state;
extern uint32_t         g_boot_time;
extern EventGroupHandle_t g_sys_events;

#define EVENT_UNLOCK_REQ        (1 << 0)
#define EVENT_LOCK_REQ          (1 << 1)
#define EVENT_LOCKOUT_START     (1 << 2)
#define EVENT_LOCKOUT_END       (1 << 3)
#define EVENT_WIFI_CONNECTED    (1 << 4)
#define EVENT_WIFI_LOST         (1 << 5)

extern QueueHandle_t    g_auth_queue;
extern QueueHandle_t    g_log_queue;
extern QueueHandle_t    g_display_queue;
extern QueueHandle_t    g_cloud_queue;
extern QueueHandle_t    g_cmd_queue;
extern SemaphoreHandle_t g_flash_mutex;
extern SemaphoreHandle_t g_user_mutex;
extern SemaphoreHandle_t g_display_mutex;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern SPI_HandleTypeDef  hspi1;
extern SPI_HandleTypeDef  hspi2;
extern I2C_HandleTypeDef  hi2c1;
extern RTC_HandleTypeDef  hrtc;

/* ======== Public Functions ======== */
void system_set_state(system_state_t new_state);
const char* auth_method_str(auth_method_t method);
const char* auth_result_str(auth_result_t result);
uint32_t get_timestamp(void);

#endif /* __MAIN_H */
