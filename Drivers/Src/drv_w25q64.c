#include "drv_w25q64.h"
#include "main.h"

/* ======== SPI 命令 ======== */
#define CMD_WRITE_ENABLE    0x06
#define CMD_READ_STATUS1    0x05
#define CMD_READ_DATA       0x03
#define CMD_PAGE_PROGRAM    0x02
#define CMD_SECTOR_ERASE    0x20
#define CMD_BLOCK_ERASE_64  0xD8
#define CMD_CHIP_ERASE      0xC7
#define CMD_JEDEC_ID        0x9F

/* ======== CS 宏 ======== */
#define CS_LOW()   HAL_GPIO_WritePin(W25Q64_Flash_CS_______GPIO_Port, \
                                     W25Q64_Flash_CS_______Pin, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(W25Q64_Flash_CS_______GPIO_Port, \
                                     W25Q64_Flash_CS_______Pin, GPIO_PIN_SET)

/* ======== 静态句柄  ======== */
static SPI_HandleTypeDef *w25q64_spi = NULL;
static uint8_t verify_buf[W25Q64_PAGE_SIZE];  /* 校验缓冲区，避任务栈 >128B 约束 */

/* ======== 内部辅助 ======== */
static int spi_tx(const uint8_t *data, uint16_t len)
{
    if (HAL_SPI_Transmit(w25q64_spi, (uint8_t *)data, len, 10) != HAL_OK)
        return W25Q64_ERR_TIMEOUT;
    return W25Q64_OK;
}

static int spi_rx(uint8_t *data, uint16_t len)
{
    if (HAL_SPI_Receive(w25q64_spi, data, len, 10) != HAL_OK)
        return W25Q64_ERR_TIMEOUT;
    return W25Q64_OK;
}

static int write_enable(void)
{
    uint8_t cmd = CMD_WRITE_ENABLE;
    CS_LOW();
    int ret = spi_tx(&cmd, 1);
    CS_HIGH();
    return ret;
}

static int read_status1(uint8_t *sr1)
{
    uint8_t cmd = CMD_READ_STATUS1;
    CS_LOW();
    int ret = spi_tx(&cmd, 1);
    if (ret == 0) ret = spi_rx(sr1, 1);
    CS_HIGH();
    return ret;
}

/* 轮询 BUSY 位直到就绪或超时 */
static int wait_ready(uint32_t timeout_ms)
{
    uint8_t sr1;
    uint32_t start = HAL_GetTick();
    do {
        if (read_status1(&sr1) != 0)
            return W25Q64_ERR_TIMEOUT;
        if (!(sr1 & 0x01))       /* BUSY = 0 → 就绪 */
            return W25Q64_OK;
    } while ((HAL_GetTick() - start) < timeout_ms);
    return W25Q64_ERR_BUSY;
}

/* ======== 公开接口 ======== */

int w25q64_init(void)
{
    w25q64_spi = &hspi2;
    CS_HIGH();

    /* 读 JEDEC ID：Winbond 0xEF, W25Q64 0x4017 */
    uint8_t cmd = CMD_JEDEC_ID;
    uint8_t id[3] = {0};

    CS_LOW();
    int ret = spi_tx(&cmd, 1);
    if (ret == 0) ret = spi_rx(id, 3);
    CS_HIGH();

    if (ret != 0) return ret;
    if (id[0] != 0xEF || id[1] != 0x40 || id[2] != 0x17)
        return W25Q64_ERR_NOT_FOUND;

    return W25Q64_OK;
}

int w25q64_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    if (!buf || len == 0 || (addr + len) > W25Q64_CAPACITY)
        return W25Q64_ERR_PARAM;

    uint8_t cmd[4] = {
        CMD_READ_DATA,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };

    CS_LOW();
    int ret = spi_tx(cmd, 4);
    if (ret == 0) ret = spi_rx(buf, (uint16_t)len);
    CS_HIGH();
    return ret;
}

int w25q64_write_page(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    if (!buf || len == 0 || len > W25Q64_PAGE_SIZE)
        return W25Q64_ERR_PARAM;
    if ((addr % W25Q64_PAGE_SIZE) + len > W25Q64_PAGE_SIZE)
        return W25Q64_ERR_PARAM;
    if ((addr + len) > W25Q64_CAPACITY)
        return W25Q64_ERR_PARAM;

    uint8_t cmd[4] = {
        CMD_PAGE_PROGRAM,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };

    /* 写入 + 回读校验，失败重试 1 次 */
    for (int attempt = 0; attempt < 2; attempt++) {
        int ret = write_enable();
        if (ret != 0) return ret;

        CS_LOW();
        ret = spi_tx(cmd, 4);
        if (ret == 0) ret = spi_tx(buf, (uint16_t)len);
        CS_HIGH();
        if (ret != 0) return ret;

        /* Page program 典型 3ms，给 10ms 超时 */
        ret = wait_ready(10);
        if (ret != 0) return ret;

        /* 回读校验 */
        ret = w25q64_read(addr, verify_buf, len);
        if (ret != 0) return ret;

        int mismatch = 0;
        for (uint32_t i = 0; i < len; i++) {
            if (verify_buf[i] != buf[i]) { mismatch = 1; break; }
        }

        if (!mismatch) return W25Q64_OK;
        /* mismatch → 重试 */
    }

    return W25Q64_ERR_VERIFY;
}

int w25q64_erase_sector(uint32_t addr)
{
    if ((addr + W25Q64_SECTOR_SIZE) > W25Q64_CAPACITY)
        return W25Q64_ERR_PARAM;

    int ret = write_enable();
    if (ret != 0) return ret;

    uint8_t cmd[4] = {
        CMD_SECTOR_ERASE,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };

    CS_LOW();
    ret = spi_tx(cmd, 4);
    CS_HIGH();
    if (ret != 0) return ret;

    return wait_ready(500);  /* 典型 45ms */
}

int w25q64_erase_64k_block(uint32_t addr)
{
    if ((addr + 0x10000) > W25Q64_CAPACITY)
        return W25Q64_ERR_PARAM;

    int ret = write_enable();
    if (ret != 0) return ret;

    uint8_t cmd[4] = {
        CMD_BLOCK_ERASE_64,
        (uint8_t)(addr >> 16),
        (uint8_t)(addr >> 8),
        (uint8_t)(addr)
    };

    CS_LOW();
    ret = spi_tx(cmd, 4);
    CS_HIGH();
    if (ret != 0) return ret;

    return wait_ready(1000);  /* 典型 150ms */
}

int w25q64_erase_chip(void)
{
    int ret = write_enable();
    if (ret != 0) return ret;

    uint8_t cmd = CMD_CHIP_ERASE;
    CS_LOW();
    ret = spi_tx(&cmd, 1);
    CS_HIGH();
    if (ret != 0) return ret;

    return wait_ready(30000);  /* 典型 20s */
}

int w25q64_is_busy(void)
{
    uint8_t sr1;
    if (read_status1(&sr1) != 0) return 1;
    return (sr1 & 0x01) ? 1 : 0;
}
