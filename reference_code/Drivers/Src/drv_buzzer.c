#include "drv_buzzer.h"
static TimerHandle_t buzzer_timer = NULL;
static void buzzer_stop_cb(TimerHandle_t x){ (void)x; HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); }
void buzzer_init(void) {
    GPIO_InitTypeDef g={0}; __HAL_RCC_GPIOB_CLK_ENABLE();
    g.Pin=GPIO_PIN_8; g.Mode=GPIO_MODE_OUTPUT_PP; g.Pull=GPIO_NOPULL; g.Speed=GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB,&g); HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
    buzzer_timer=xTimerCreate("Buz",pdMS_TO_TICKS(100),pdFALSE,NULL,buzzer_stop_cb);
}
void buzzer_beep(uint32_t ms) {
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
    if(buzzer_timer){ xTimerChangePeriod(buzzer_timer,pdMS_TO_TICKS(ms),0); xTimerStart(buzzer_timer,0); }
}
void buzzer_beep_n(uint8_t n, uint32_t interval_ms) {
    for(uint8_t i=0;i<n;i++){ buzzer_beep(interval_ms/2); HAL_Delay(interval_ms); }
}
void buzzer_alarm(void) { HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET); if(buzzer_timer) xTimerStop(buzzer_timer,0); }
void buzzer_stop(void) { HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET); if(buzzer_timer) xTimerStop(buzzer_timer,0); }
