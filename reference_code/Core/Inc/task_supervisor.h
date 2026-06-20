#ifndef __TASK_SUPERVISOR_H
#define __TASK_SUPERVISOR_H
#include "main.h"
void task_supervisor_entry(void *pvParameters);
void supervisor_feed_dog(void);
uint32_t supervisor_get_uptime(void);
#endif
