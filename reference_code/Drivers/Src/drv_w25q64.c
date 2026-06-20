#include "drv_w25q64.h"
#define CS_LOW()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)
#define CMD_WREN  0x06
#define CMD_READ  0x03
#define CMD_PP    0x02
#define CMD_SE    0x20
#define CMD_CE    0xC7
#define CMD_RDSR1 0x05
static SPI_HandleTypeDef *w25=NULL;
static void wait_busy(void){ uint8_t s; do{CS_LOW(); uint8_t c=CMD_RDSR1; HAL_SPI_Transmit(w25,&c,1,100); HAL_SPI_Receive(w25,&s,1,100); CS_HIGH(); }while(s&1); }
static void wren(void){ CS_LOW(); uint8_t c=CMD_WREN; HAL_SPI_Transmit(w25,&c,1,100); CS_HIGH(); }
void w25q64_init(SPI_HandleTypeDef *h){ w25=h; GPIO_InitTypeDef g={0}; __HAL_RCC_GPIOA_CLK_ENABLE(); g.Pin=GPIO_PIN_4; g.Mode=GPIO_MODE_OUTPUT_PP; g.Pull=GPIO_PULLUP; g.Speed=GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(GPIOA,&g); CS_HIGH(); }
void w25q64_read(uint32_t a,uint8_t *b,uint32_t n){ uint8_t c[4]={CMD_READ,(uint8_t)(a>>16),(uint8_t)(a>>8),(uint8_t)a}; CS_LOW(); HAL_SPI_Transmit(w25,c,4,100); HAL_SPI_Receive(w25,b,n,HAL_MAX_DELAY); CS_HIGH(); }
void w25q64_write_page(uint32_t a,uint8_t *b,uint32_t n){ if(n>256)n=256; uint8_t c[4]={CMD_PP,(uint8_t)(a>>16),(uint8_t)(a>>8),(uint8_t)a}; wren(); CS_LOW(); HAL_SPI_Transmit(w25,c,4,100); HAL_SPI_Transmit(w25,b,n,HAL_MAX_DELAY); CS_HIGH(); wait_busy(); }
void w25q64_erase_sector(uint32_t a){ uint8_t c[4]={CMD_SE,(uint8_t)(a>>16),(uint8_t)(a>>8),(uint8_t)a}; wren(); CS_LOW(); HAL_SPI_Transmit(w25,c,4,100); CS_HIGH(); wait_busy(); }
void w25q64_read_ex(uint32_t a,uint8_t *b,uint32_t n){ while(n){ uint32_t ch=n; if(a%256+ch>256)ch=256-(a%256); w25q64_read(a,b,ch); a+=ch; b+=ch; n-=ch; } }
void w25q64_write_ex(uint32_t a,uint8_t *b,uint32_t n){ while(n){ uint32_t ch=n; if(a%256+ch>256)ch=256-(a%256); w25q64_write_page(a,b,ch); a+=ch; b+=ch; n-=ch; } }
