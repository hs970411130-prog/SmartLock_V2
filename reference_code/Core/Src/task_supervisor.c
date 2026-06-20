#include "task_supervisor.h"
static IWDG_HandleTypeDef hiwdg;
static uint32_t uptime=0;
void task_supervisor_entry(void *pv){ (void)pv;
    hiwdg.Instance=IWDG; hiwdg.Init.Prescaler=IWDG_PRESCALER_64; hiwdg.Init.Reload=4095; HAL_IWDG_Init(&hiwdg);
    uint32_t ls=HAL_GetTick(), lw=0;
    for(;;){ vTaskDelayUntil(&lw,pdMS_TO_TICKS(1000)); HAL_IWDG_Refresh(&hiwdg); uint32_t n=HAL_GetTick(); uptime+=(n-ls)/1000; ls=n; }
}
void supervisor_feed_dog(void){ HAL_IWDG_Refresh(&hiwdg); }
uint32_t supervisor_get_uptime(void){ return uptime; }
