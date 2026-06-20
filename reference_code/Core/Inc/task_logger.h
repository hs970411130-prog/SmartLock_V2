#ifndef __TASK_LOGGER_H
#define __TASK_LOGGER_H
#include "main.h"
void task_logger_entry(void *pvParameters);
int  logger_add(log_type_t type, uint32_t user_id, auth_method_t method, const char *detail);
int  logger_read_recent(log_entry_t *entries, uint16_t max_count);
uint32_t logger_get_count(void);
void logger_clear(void);
#endif
