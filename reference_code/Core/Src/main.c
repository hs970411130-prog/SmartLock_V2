#include "main.h"
#include "task_auth.h"
#include "task_display.h"
#include "task_cloud.h"
#include "task_keypad.h"
#include "task_logger.h"
#include "task_supervisor.h"
#include "drv_relay.h"
#include "drv_buzzer.h"

system_state_t g_sys_state = STATE_IDLE;
uint32_t g_boot_time = 0;
EventGroupHandle_t g_sys_events = NULL;
QueueHandle_t g_auth_queue = NULL, g_log_queue = NULL, g_display_queue = NULL, g_cloud_queue = NULL, g_cmd_queue = NULL;
SemaphoreHandle_t g_flash_mutex = NULL, g_user_mutex = NULL, g_display_mutex = NULL;
UART_HandleTypeDef huart1, huart2, huart3;
SPI_HandleTypeDef hspi1, hspi2;
I2C_HandleTypeDef hi2c1;
RTC_HandleTypeDef hrtc;

void system_set_state(system_state_t s){ g_sys_state=s; }
const char* auth_method_str(auth_method_t m){ switch(m){ case AUTH_METHOD_FINGERPRINT:return "fingerprint"; case AUTH_METHOD_RFID:return "RFID"; case AUTH_METHOD_PASSWORD:return "password"; default:return "unknown"; } }
const char* auth_result_str(auth_result_t r){ switch(r){ case AUTH_SUCCESS:return "success"; case AUTH_FAIL_WRONG:return "wrong"; case AUTH_FAIL_LOCKOUT:return "lockout"; default:return "unknown"; } }
uint32_t get_timestamp(void){ return HAL_GetTick()+g_boot_time; }

static void SystemClock_Config(void){ /* CubeMX generated */ }
static void MX_GPIO_Init(void){ __HAL_RCC_GPIOA_CLK_ENABLE(); __HAL_RCC_GPIOB_CLK_ENABLE(); __HAL_RCC_GPIOC_CLK_ENABLE(); __HAL_RCC_GPIOD_CLK_ENABLE(); __HAL_RCC_GPIOE_CLK_ENABLE(); }

int main(void){
    HAL_Init(); SystemClock_Config(); MX_GPIO_Init();
    g_boot_time=HAL_GetTick();
    g_sys_events=xEventGroupCreate();
    g_auth_queue=xQueueCreate(10,sizeof(auth_request_t));
    g_log_queue=xQueueCreate(20,sizeof(log_entry_t));
    g_display_queue=xQueueCreate(10,sizeof(uint8_t));
    g_cloud_queue=xQueueCreate(10,sizeof(uint8_t));
    g_cmd_queue=xQueueCreate(5,sizeof(uint8_t));
    g_flash_mutex=xSemaphoreCreateMutex();
    g_user_mutex=xSemaphoreCreateMutex();
    g_display_mutex=xSemaphoreCreateMutex();
    relay_init(); buzzer_init();
    xTaskCreate(task_auth_entry,"Auth",2048,NULL,5,NULL);
    xTaskCreate(task_display_entry,"Disp",512,NULL,2,NULL);
    xTaskCreate(task_keypad_entry,"Keypad",512,NULL,1,NULL);
    xTaskCreate(task_logger_entry,"Logger",1024,NULL,3,NULL);
    xTaskCreate(task_cloud_entry,"Cloud",1024,NULL,4,NULL);
    xTaskCreate(task_supervisor_entry,"Super",256,NULL,0,NULL);
    vTaskStartScheduler();
    while(1);
}
