#ifndef __DRV_W25Q64_H
#define __DRV_W25Q64_H
#include <stdint.h>

/* ======== 私有错误码（App 层只判 0 / 非 0） ======== */
#define W25Q64_OK               0
#define W25Q64_ERR_TIMEOUT     -1
#define W25Q64_ERR_NOT_FOUND   -2
#define W25Q64_ERR_VERIFY      -3
#define W25Q64_ERR_PARAM       -4
#define W25Q64_ERR_BUSY        -5

/* ======== 硬件参数 ======== */
#define W25Q64_PAGE_SIZE        256
#define W25Q64_SECTOR_SIZE      4096
#define W25Q64_CAPACITY         0x800000   // 8MB

/* ======== 接口 ======== */
int  w25q64_init(void);
int  w25q64_read(uint32_t addr, uint8_t *buf, uint32_t len);
int  w25q64_write_page(uint32_t addr, const uint8_t *buf, uint32_t len);
int  w25q64_erase_sector(uint32_t addr);
int  w25q64_erase_64k_block(uint32_t addr);
int  w25q64_erase_chip(void);
int  w25q64_is_busy(void);

#endif /* __DRV_W25Q64_H */
