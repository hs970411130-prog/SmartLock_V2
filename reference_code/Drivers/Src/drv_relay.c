#include "drv_relay.h"
static uint8_t is_unlocked = 0;
static TimerHandle_t relay_timer = NULL;
static void relay_auto_lock(TimerHandle_t xTimer) {
    (void)xTimer; RELAY_OFF(); is_unlocked = 0;
}
void relay_init(void) {
    GPIO_InitTypeDef g = {0};
    __HAL_RCC_GPIOE_CLK_ENABLE();
    g.Pin=GPIO_PIN_0; g.Mode=GPIO_MODE_OUTPUT_PP; g.Pull=GPIO_NOPULL; g.Speed=GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE,&g); RELAY_OFF();
    relay_timer = xTimerCreate("Rly",pdMS_TO_TICKS(100),pdFALSE,NULL,relay_auto_lock);
}
void relay_unlock(uint32_t ms) {
    if(!ms) return; RELAY_ON(); is_unlocked=1;
    if(relay_timer){ xTimerChangePeriod(relay_timer,pdMS_TO_TICKS(ms),0); xTimerStart(relay_timer,0); }
}
int relay_is_unlocked(void) { return is_unlocked; }
