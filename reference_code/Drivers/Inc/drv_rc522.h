#ifndef __DRV_RC522_H
#define __DRV_RC522_H
#include "main.h"
void rc522_init(SPI_HandleTypeDef *hspi);
int  rc522_check_card(uint8_t *uid, uint8_t *uid_len);
int  rc522_read_block(uint8_t block_addr, uint8_t *data);
void rc522_halt(void);
uint8_t rc522_read_reg(uint8_t reg);
void    rc522_write_reg(uint8_t reg, uint8_t val);
#endif
