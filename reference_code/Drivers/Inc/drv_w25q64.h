#ifndef __DRV_W25Q64_H
#define __DRV_W25Q64_H
#include "main.h"
#define W25Q64_PAGE_SIZE    256
#define W25Q64_SECTOR_SIZE  4096
void w25q64_init(SPI_HandleTypeDef *hspi);
void w25q64_read(uint32_t addr, uint8_t *buf, uint32_t len);
void w25q64_write_page(uint32_t addr, uint8_t *buf, uint32_t len);
void w25q64_erase_sector(uint32_t addr);
void w25q64_read_ex(uint32_t addr, uint8_t *buf, uint32_t len);
void w25q64_write_ex(uint32_t addr, uint8_t *buf, uint32_t len);
#endif
