#ifndef __DRV_RELAY_H
#define __DRV_RELAY_H
#include "main.h"
#define RELAY_ON()  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET)
#define RELAY_OFF() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET)
void relay_init(void);
void relay_unlock(uint32_t duration_ms);
int  relay_is_unlocked(void);
#endif
