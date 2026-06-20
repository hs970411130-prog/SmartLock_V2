#ifndef __DRV_BUZZER_H
#define __DRV_BUZZER_H
#include "main.h"
void buzzer_init(void);
void buzzer_beep(uint32_t duration_ms);
void buzzer_beep_n(uint8_t count, uint32_t interval_ms);
void buzzer_alarm(void);
void buzzer_stop(void);
#endif
