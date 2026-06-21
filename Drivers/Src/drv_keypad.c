#include "drv_keypad.h"
#include "main.h"

/* 4×4 矩阵：PC0-PC3 行输出，PC4-PC7 列输入（下拉） */
/* 扫描策略：所有行初始 LOW，逐行拉高后读列 */
static const uint16_t row_pins[4] = {ROW0_Pin, ROW1_Pin, ROW2_Pin, ROW3_Pin};
static const uint16_t col_pins[4] = {COLUMN0_Pin, COLUMN1_Pin, COLUMN2_Pin, COLUMN3_Pin};

static const uint8_t keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'},
};

void keypad_init(void)
{
    /* CubeMX 初始化为行 HIGH，这里全部拉低以适配下拉列输入扫描 */
    for (int r = 0; r < 4; r++)
        HAL_GPIO_WritePin(GPIOC, row_pins[r], GPIO_PIN_RESET);
}

int keypad_scan(uint8_t *key_out)
{
    if (!key_out) return KEYPAD_ERR;
    *key_out = KEYPAD_NONE;

    for (int row = 0; row < 4; row++) {
        /* 当前行拉高 */
        HAL_GPIO_WritePin(GPIOC, row_pins[row], GPIO_PIN_SET);

        /* 短暂延时等电平稳定 */
        for (volatile int d = 0; d < 10; d++);

        for (int col = 0; col < 4; col++) {
            if (HAL_GPIO_ReadPin(GPIOC, col_pins[col]) == GPIO_PIN_SET) {
                /* 列被按键拉到高电平 */
                *key_out = keymap[row][col];
                HAL_GPIO_WritePin(GPIOC, row_pins[row], GPIO_PIN_RESET);
                return KEYPAD_OK;
            }
        }

        /* 恢复该行低电平 */
        HAL_GPIO_WritePin(GPIOC, row_pins[row], GPIO_PIN_RESET);
    }

    return KEYPAD_ERR;
}
