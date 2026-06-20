#ifndef __TASK_AUTH_H
#define __TASK_AUTH_H
#include "main.h"
void task_auth_entry(void *pvParameters);
int  auth_submit_request(auth_request_t *request);
uint8_t auth_get_retry_count(void);
int  auth_admin_login(const char *password);
int  auth_register_user(user_record_t *user);
int  auth_delete_user(uint32_t user_id);
int  auth_unlock_door(auth_method_t method, uint32_t user_id);
int  auth_lock_door(void);
#endif
