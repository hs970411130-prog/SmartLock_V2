#ifndef __TASK_DISPLAY_H
#define __TASK_DISPLAY_H
#include "main.h"
void task_display_entry(void *pvParameters);
void display_set_page(display_page_t page);
void display_show_auth_result(auth_result_t result, auth_method_t method, const char *username);
void display_show_countdown(const char *msg, uint32_t remaining_ms);
void display_show_toast(const char *msg);
void display_show_home(void);
#endif
