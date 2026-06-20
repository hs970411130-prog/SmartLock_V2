#include "task_logger.h"
#include "drv_w25q64.h"
typedef struct { uint32_t magic,total,head; uint8_t rsv[240]; } lh_t;
#define LM 0x4C4F4700
#define LS W25Q64_SECTOR_SIZE
#define LE 64
static lh_t lh;
static void init_flash(void){ xSemaphoreTake(g_flash_mutex,portMAX_DELAY); w25q64_read(FLASH_LOG_ADDR,(uint8_t*)&lh,sizeof(lh)); if(lh.magic!=LM){ w25q64_erase_sector(FLASH_LOG_ADDR); lh.magic=LM; lh.total=0; lh.head=0; memset(lh.rsv,0,240); w25q64_write_page(FLASH_LOG_ADDR,(uint8_t*)&lh,sizeof(lh)); } xSemaphoreGive(g_flash_mutex); }
static void save_hdr(void){ xSemaphoreTake(g_flash_mutex,portMAX_DELAY); w25q64_erase_sector(FLASH_LOG_ADDR); w25q64_write_page(FLASH_LOG_ADDR,(uint8_t*)&lh,sizeof(lh)); xSemaphoreGive(g_flash_mutex); }
int logger_add(log_type_t t,uint32_t uid,auth_method_t m,const char *d){ log_entry_t e; memset(&e,0,sizeof(e)); e.type=t; e.timestamp=get_timestamp(); e.user_id=uid; e.method=m; if(d)strncpy(e.detail,d,31); uint32_t off=FLASH_LOG_ADDR+LS+lh.head*LE; if(off+LE>FLASH_USER_ADDR){ lh.head=0; off=FLASH_LOG_ADDR+LS; xSemaphoreTake(g_flash_mutex,portMAX_DELAY); w25q64_erase_sector(FLASH_LOG_ADDR+LS); xSemaphoreGive(g_flash_mutex); } xSemaphoreTake(g_flash_mutex,portMAX_DELAY); w25q64_write_page(off,(uint8_t*)&e,sizeof(e)); xSemaphoreGive(g_flash_mutex); lh.head++; lh.total++; if(lh.head>=MAX_LOG_ENTRIES)lh.head=0; save_hdr(); return 0; }
uint32_t logger_get_count(void){ return lh.total; }
void logger_clear(void){ lh.total=0; lh.head=0; save_hdr(); w25q64_erase_sector(FLASH_LOG_ADDR+LS); }
void task_logger_entry(void *pv){ (void)pv; init_flash(); logger_add(LOG_SYSTEM_BOOT,0,AUTH_METHOD_NONE,"boot"); for(;;)vTaskDelay(pdMS_TO_TICKS(10000)); }
