#ifndef __APP_TYPES_H
#define __APP_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/* ======== 认证方式 ======== */
typedef enum {
    AUTH_TYPE_NONE = 0,
    AUTH_TYPE_FINGERPRINT = 1,
    AUTH_TYPE_RFID = 2,
    AUTH_TYPE_PASSWORD = 3,
    AUTH_TYPE_ADMIN = 4,
} auth_type_t;

/* ======== 认证结果 ======== */
typedef enum {
    AUTH_RESULT_SUCCESS = 0,
    AUTH_RESULT_FAIL_WRONG = 1,
    AUTH_RESULT_FAIL_LOCKOUT = 2,
    AUTH_RESULT_FAIL_NOT_FOUND = 3,
    AUTH_RESULT_FAIL_TIMEOUT = 4,
} auth_result_t;

/* ======== 认证请求 ======== */
typedef struct {
    auth_type_t type;
    union {
        uint16_t fingerprint_id;
        uint8_t  rfid_uid[4];
        char     password[16];
    } credential;
    uint32_t user_id;
    uint32_t timestamp_ms;
} auth_request_t;

/* ======== 用户角色 ======== */
typedef enum {
    ROLE_GUEST = 0,
    ROLE_USER = 1,
    ROLE_ADMIN = 2,
} user_role_t;

/* ======== 用户记录（Flash 持久化，data_ver 必须为首成员） ======== */
typedef struct {
    uint32_t data_ver;
    uint32_t id;
    char     name[16];
    uint8_t  rfid_uid[4];
    char     password_hash[32];
    uint8_t  fp_ids[5];
    uint8_t  fp_count;
    user_role_t role;
    uint32_t created_time;
    uint32_t expire_time;
    uint8_t  enabled;
} user_record_t;

/* ======== 日志类型 ======== */
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

/* ======== 日志条目（Flash 持久化，data_ver 必须为首成员） ======== */
typedef struct {
    uint32_t data_ver;
    log_type_t type;
    uint32_t timestamp;
    uint32_t user_id;
    auth_type_t method;
    char     detail[32];
} log_entry_t;

/* ======== 系统状态机 ======== */
typedef enum {
    STATE_IDLE = 0,
    STATE_AUTHENTICATING = 1,
    STATE_UNLOCKED = 2,
    STATE_LOCKED_OUT = 3,
} sys_state_t;

/* ======== 显示页面 ======== */
typedef enum {
    PAGE_HOME = 0,
    PAGE_AUTH_RESULT = 1,
    PAGE_SETTINGS = 2,
    PAGE_LOG_VIEW = 3,
} display_page_t;

#endif /* __APP_TYPES_H */
