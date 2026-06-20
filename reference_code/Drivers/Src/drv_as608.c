#include "drv_as608.h"
static UART_HandleTypeDef *a_u=NULL;
static uint8_t rx[256];
static volatile uint16_t rxi=0;
static uint16_t chksum(uint8_t *p,uint16_t n){ uint16_t s=0; for(uint16_t i=6;i<n-2;i++)s+=p[i]; return s; }
int as608_send_command(uint8_t *cmd,uint16_t cl,uint8_t *resp,uint16_t *rl,uint32_t to){
    uint16_t tl=2+4+1+2+cl+2, idx=0;
    uint8_t p[128];
    p[idx++]=0xEF; p[idx++]=0x01;
    p[idx++]=0xFF; p[idx++]=0xFF; p[idx++]=0xFF; p[idx++]=0xFF;
    p[idx++]=0x01;
    uint16_t lf=cl+2; p[idx++]=(lf>>8)&0xFF; p[idx++]=lf&0xFF;
    if(cmd&&cl){ memcpy(&p[idx],cmd,cl); idx+=cl; }
    uint16_t ck=chksum(p,idx); p[idx++]=(ck>>8)&0xFF; p[idx++]=ck&0xFF;
    rxi=0; HAL_UART_Transmit(a_u,p,idx,100);
    uint32_t st=HAL_GetTick();
    while(rxi<12){ if(HAL_GetTick()-st>to)return -1; }
    if(rx[0]!=0xEF||rx[1]!=0x01)return -2;
    if(rx[6]!=0x07)return -3;
    uint8_t cf=rx[9];
    if(resp&&rl&&rxi>12){ *rl=rxi-12; memcpy(resp,&rx[12],*rl); }
    return (cf==0)?0:cf;
}
int as608_init(UART_HandleTypeDef *h){ a_u=h; rxi=0; uint8_t c[]={0x01,0x00,0x03,0x0D,0x00,0x05}; uint8_t r[32]; uint16_t rl=0; return as608_send_command(c,sizeof(c),r,&rl,500); }
int as608_detect_finger(void){ uint8_t c[]={0x01,0x00,0x03,0x01,0x00,0x05}; int r=as608_send_command(c,sizeof(c),NULL,NULL,3000); return(!r); }
int as608_capture_image(uint8_t bid){ uint8_t c[]={0x01,0x00,0x04,0x02,bid,0x00,(uint8_t)(0x07+bid)}; return as608_send_command(c,sizeof(c),NULL,NULL,1000); }
int as608_create_template(void){ uint8_t c[]={0x01,0x00,0x03,0x05,0x00,0x09}; return as608_send_command(c,sizeof(c),NULL,NULL,1000); }
int as608_store_template(uint16_t idx){ uint8_t c[]={0x01,0x00,0x06,0x06,0x01,(uint8_t)(idx>>8),(uint8_t)idx,0x00,(uint8_t)(0x0E+(idx>>8)+(idx&0xFF))}; return as608_send_command(c,sizeof(c),NULL,NULL,2000); }
int as608_search_fingerprint(uint16_t *id,uint16_t *sc){ uint8_t c[]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,MAX_FINGERPRINT_COUNT,0x00,(uint8_t)(0x0E+MAX_FINGERPRINT_COUNT)}; uint8_t r[16]; uint16_t rl=0; int ret=as608_send_command(c,sizeof(c),r,&rl,3000); if(!ret&&rl>=4){ *id=(r[0]<<8)|r[1]; *sc=(r[2]<<8)|r[3]; return 0; } return ret; }
int as608_delete_fingerprint(uint16_t idx){ uint8_t c[]={0x01,0x00,0x07,0x0C,(uint8_t)(idx>>8),(uint8_t)idx,0x00,0x01,0x00,(uint8_t)(0x15+(idx>>8)+(idx&0xFF))}; return as608_send_command(c,sizeof(c),NULL,NULL,1000); }
int as608_clear_all(void){ uint8_t c[]={0x01,0x00,0x03,0x0D,0x00,0x11}; return as608_send_command(c,sizeof(c),NULL,NULL,1000); }
int as608_get_count(uint16_t *cnt){ uint8_t c[]={0x01,0x00,0x03,0x1D,0x00,0x21}; uint8_t r[32]; uint16_t rl=0; int ret=as608_send_command(c,sizeof(c),r,&rl,1000); if(!ret&&rl>=16){ *cnt=(r[10]<<8)|r[11]; return 0; } return ret; }
void as608_uart_rx_callback(void){ HAL_UART_Receive_IT(a_u,&rx[rxi],1); rxi++; if(rxi>=sizeof(rx))rxi=0; }
