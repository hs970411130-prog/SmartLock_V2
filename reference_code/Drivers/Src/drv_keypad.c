#include "drv_keypad.h"
static const uint16_t rp[4]={GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7};
static const uint16_t cp[4]={GPIO_PIN_8,GPIO_PIN_9,GPIO_PIN_10,GPIO_PIN_11};
static const uint8_t km[4][4]={{KEY_1,KEY_2,KEY_3,KEY_A},{KEY_4,KEY_5,KEY_6,KEY_B},{KEY_7,KEY_8,KEY_9,KEY_C},{KEY_STAR,KEY_0,KEY_HASH,KEY_D}};
static uint8_t last_key=KEY_NONE, db=0;
void keypad_init(void) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
    GPIO_InitTypeDef g={0};
    g.Pin=GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7; g.Mode=GPIO_MODE_INPUT; g.Pull=GPIO_PULLUP; HAL_GPIO_Init(GPIOE,&g);
    g.Pin=GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11; g.Mode=GPIO_MODE_OUTPUT_PP; g.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE,&g);
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11,GPIO_PIN_SET);
}
uint8_t keypad_scan(void) {
    for(uint8_t c=0;c<4;c++){ HAL_GPIO_WritePin(GPIOE,cp[c],GPIO_PIN_RESET);
        for(volatile int d=0;d<100;d++);
        for(uint8_t r=0;r<4;r++){ if(HAL_GPIO_ReadPin(GPIOE,rp[r])==GPIO_PIN_RESET){ HAL_GPIO_WritePin(GPIOE,cp[c],GPIO_PIN_SET); return km[r][c]; } }
        HAL_GPIO_WritePin(GPIOE,cp[c],GPIO_PIN_SET); }
    return KEY_NONE;
}
uint8_t keypad_scan_debounced(void) {
    uint8_t k=keypad_scan();
    if(k!=KEY_NONE){ if(k==last_key){ db++; if(db>=2){ db=0; return k; } } else { last_key=k; db=1; } return KEY_NONE; }
    else { last_key=KEY_NONE; db=0; return KEY_NONE; }
}
char keypad_to_ascii(uint8_t k){ if(k<=9)return '0'+k; switch(k){ case KEY_A:return 'A'; case KEY_B:return 'B'; case KEY_C:return 'C'; case KEY_D:return 'D'; case KEY_STAR:return '*'; case KEY_HASH:return '#'; default:return '?'; } }
int keypad_is_digit(uint8_t k){ return k<=9; }
