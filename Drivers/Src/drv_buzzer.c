#include "drv_buzzer.h"
#include "stm32f4xx_hal.h"

/* PE1 = 蜂鸣器，低电平发声，高电平静音 */
/* GPIO 初始化由 CubeMX MX_GPIO_Init() 完成 */

static GPIO_TypeDef *buzzer_port = GPIOE;
static uint16_t      buzzer_pin  = GPIO_PIN_1;

void buzzer_init(void)
{
    /* 上电即静音 */
    HAL_GPIO_WritePin(buzzer_port, buzzer_pin, GPIO_PIN_SET);
}

void buzzer_on(void)
{
    HAL_GPIO_WritePin(buzzer_port, buzzer_pin, GPIO_PIN_RESET);
}

void buzzer_off(void)
{
    HAL_GPIO_WritePin(buzzer_port, buzzer_pin, GPIO_PIN_SET);
}
