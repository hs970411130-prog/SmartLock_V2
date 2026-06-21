#ifndef __DRV_KEYPAD_H
#define __DRV_KEYPAD_H
#include <stdint.h>

#define KEYPAD_NONE  0xFF

/* 私有错误码（App 层只判 0 / 非 0） */
#define KEYPAD_OK    0
#define KEYPAD_ERR  -1

void keypad_init(void);
int  keypad_scan(uint8_t *key_out);   /* 单次扫描，非阻塞 */

#endif /* __DRV_KEYPAD_H */
