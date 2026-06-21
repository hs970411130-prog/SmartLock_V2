#include "drv_relay.h"
#include "stm32f4xx_hal.h"

/* PE0 = 继电器，低电平开锁，高电平锁定 */
/* GPIO 初始化由 CubeMX MX_GPIO_Init() 完成 */

static GPIO_TypeDef *relay_port = GPIOE;
static uint16_t      relay_pin  = GPIO_PIN_0;
static bool          unlocked   = false;

void relay_init(void)
{
    /* 上电即锁定 */
    HAL_GPIO_WritePin(relay_port, relay_pin, GPIO_PIN_SET);
    unlocked = false;
}

void relay_unlock(void)
{
    HAL_GPIO_WritePin(relay_port, relay_pin, GPIO_PIN_RESET);
    unlocked = true;
}

void relay_lock(void)
{
    HAL_GPIO_WritePin(relay_port, relay_pin, GPIO_PIN_SET);
    unlocked = false;
}

bool relay_is_unlocked(void)
{
    return unlocked;
}
