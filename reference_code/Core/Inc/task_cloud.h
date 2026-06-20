#ifndef __TASK_CLOUD_H
#define __TASK_CLOUD_H
#include "main.h"
void task_cloud_entry(void *pvParameters);
int  cloud_report_unlock(uint32_t user_id, auth_method_t method);
int  cloud_report_alarm(const char *reason);
#endif
