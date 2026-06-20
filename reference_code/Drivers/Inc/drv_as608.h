#ifndef __DRV_AS608_H
#define __DRV_AS608_H
#include "main.h"
int as608_init(UART_HandleTypeDef *huart);
int as608_detect_finger(void);
int as608_capture_image(uint8_t buffer_id);
int as608_create_template(void);
int as608_store_template(uint16_t index);
int as608_search_fingerprint(uint16_t *matched_id, uint16_t *score);
int as608_delete_fingerprint(uint16_t index);
int as608_clear_all(void);
int as608_get_count(uint16_t *count);
void as608_uart_rx_callback(void);
#endif
