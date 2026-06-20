#include "drv_rc522.h"
static SPI_HandleTypeDef *r_spi=NULL;
#define CSL() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)
#define CSH() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)
#define AW(r) (((r)<<1)&0x7E)
#define AR(r) ((((r)<<1)&0x7E)|0x80)
#define R_CMD    0x01
#define R_COMIRQ 0x04
#define R_FIFOD  0x09
#define R_FIFOLV 0x0A
#define R_BITFRM 0x0D
#define R_MODE   0x11
#define R_TXCTL  0x14
#define R_TMODE  0x2A
#define R_TPRES  0x2B
#define R_TRLDH  0x2C
#define R_TRLDL  0x2D
uint8_t rc522_read_reg(uint8_t r){ uint8_t a=AR(r),v=0; CSL(); HAL_SPI_TransmitReceive(r_spi,&a,&v,1,100); CSH(); return v; }
void rc522_write_reg(uint8_t r,uint8_t v){ uint8_t b[2]={AW(r),v}; CSL(); HAL_SPI_Transmit(r_spi,b,2,100); CSH(); }
void rc522_init(SPI_HandleTypeDef *h){ r_spi=h; GPIO_InitTypeDef g={0}; __HAL_RCC_GPIOB_CLK_ENABLE(); g.Pin=GPIO_PIN_12; g.Mode=GPIO_MODE_OUTPUT_PP; g.Pull=GPIO_PULLUP; g.Speed=GPIO_SPEED_FREQ_HIGH; HAL_GPIO_Init(GPIOB,&g); CSH(); rc522_write_reg(R_CMD,0x0F); HAL_Delay(50); rc522_write_reg(R_TMODE,0x80); rc522_write_reg(R_TPRES,0xA9); rc522_write_reg(R_TRLDH,0x03); rc522_write_reg(R_TRLDL,0xE8); rc522_write_reg(R_MODE,0x3D); uint8_t v=rc522_read_reg(R_TXCTL); rc522_write_reg(R_TXCTL,v|0x03); }
static int xfer(uint8_t *sd,uint8_t sl,uint8_t *rd,uint8_t *rl){
    rc522_write_reg(0x02,0x77|0x80); rc522_write_reg(R_COMIRQ,0x7F); rc522_write_reg(R_CMD,0x00);
    rc522_write_reg(R_FIFOLV,0x80); for(uint8_t i=0;i<sl;i++)rc522_write_reg(R_FIFOD,sd[i]);
    rc522_write_reg(R_CMD,0x0C); rc522_write_reg(R_BITFRM,0x80);
    uint32_t dl=HAL_GetTick()+50; uint8_t irq;
    do{ irq=rc522_read_reg(R_COMIRQ); if(HAL_GetTick()>dl)return-1; }while(!(irq&0x30)&&!(irq&1));
    if(irq&1||irq&2)return-2;
    uint8_t fl=rc522_read_reg(R_FIFOLV); if(rd&&rl&&fl){ *rl=fl>64?64:fl; for(uint8_t i=0;i<*rl;i++)rd[i]=rc522_read_reg(R_FIFOD); }
    return 0;
}
int rc522_check_card(uint8_t *uid,uint8_t *ul){
    uint8_t rb[64],rl=0, reqa=0x26; if(xfer(&reqa,1,rb,&rl))return-1; if(rl!=2)return-2;
    uint8_t ac[2]={0x93,0x20}; if(xfer(ac,2,rb,&rl))return-3; if(rl<5)return-4;
    uint8_t x=0; for(int i=0;i<4;i++)x^=rb[i]; if(x!=rb[4])return-5;
    if(uid)memcpy(uid,rb,4); if(ul)*ul=4;
    uint8_t sel[7]={0x93,0x70,rb[0],rb[1],rb[2],rb[3],rb[4]}; if(xfer(sel,7,rb,&rl))return-6;
    return 0;
}
int rc522_read_block(uint8_t ba,uint8_t *d){ uint8_t c[2]={0x30,ba},rb[20],rl=0; int r=xfer(c,2,rb,&rl); if(r||rl!=18)return-1; memcpy(d,rb,16); return 0; }
void rc522_halt(void){ uint8_t c[2]={0x50,0x00},d[4],dl=0; xfer(c,2,d,&dl); }
