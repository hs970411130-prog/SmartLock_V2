#ifndef __DRV_KEYPAD_H
#define __DRV_KEYPAD_H
#include "main.h"
#define KEY_NONE 0xFF
#define KEY_1 0x01
#define KEY_2 0x02
#define KEY_3 0x03
#define KEY_A 0x0A
#define KEY_4 0x04
#define KEY_5 0x05
#define KEY_6 0x06
#define KEY_B 0x0B
#define KEY_7 0x07
#define KEY_8 0x08
#define KEY_9 0x09
#define KEY_C 0x0C
#define KEY_STAR 0x0D
#define KEY_0 0x00
#define KEY_HASH 0x0E
#define KEY_D 0x0F
#define KEY_CONFIRM KEY_C
#define KEY_CANCEL  KEY_B
#define KEY_CLEAR   KEY_STAR
#define KEY_ADMIN   KEY_HASH
void keypad_init(void);
uint8_t keypad_scan(void);
uint8_t keypad_scan_debounced(void);
char keypad_to_ascii(uint8_t key);
int  keypad_is_digit(uint8_t key);
#endif
