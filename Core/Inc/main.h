/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ROW0_Pin GPIO_PIN_0
#define ROW0_GPIO_Port GPIOC
#define ROW1_Pin GPIO_PIN_1
#define ROW1_GPIO_Port GPIOC
#define ROW2_Pin GPIO_PIN_2
#define ROW2_GPIO_Port GPIOC
#define ROW3_Pin GPIO_PIN_3
#define ROW3_GPIO_Port GPIOC
#define ESP8266_WiFi_TX_Pin GPIO_PIN_2
#define ESP8266_WiFi_TX_GPIO_Port GPIOA
#define ESP8266_WiFi_RX_Pin GPIO_PIN_3
#define ESP8266_WiFi_RX_GPIO_Port GPIOA
#define RC522_RFID_CS_______Pin GPIO_PIN_4
#define RC522_RFID_CS_______GPIO_Port GPIOA
#define RC522_RFID_SCK_Pin GPIO_PIN_5
#define RC522_RFID_SCK_GPIO_Port GPIOA
#define RC522_RFID_MISO_Pin GPIO_PIN_6
#define RC522_RFID_MISO_GPIO_Port GPIOA
#define RC522_RFID_MOSI_Pin GPIO_PIN_7
#define RC522_RFID_MOSI_GPIO_Port GPIOA
#define COLUMN0_Pin GPIO_PIN_4
#define COLUMN0_GPIO_Port GPIOC
#define COLUMN1_Pin GPIO_PIN_5
#define COLUMN1_GPIO_Port GPIOC
#define TX_____Pin GPIO_PIN_10
#define TX_____GPIO_Port GPIOB
#define RX_____Pin GPIO_PIN_11
#define RX_____GPIO_Port GPIOB
#define W25Q64_Flash_CS_______Pin GPIO_PIN_12
#define W25Q64_Flash_CS_______GPIO_Port GPIOB
#define W25Q64_Flash_SCK_Pin GPIO_PIN_13
#define W25Q64_Flash_SCK_GPIO_Port GPIOB
#define W25Q64_Flash_MISO_Pin GPIO_PIN_14
#define W25Q64_Flash_MISO_GPIO_Port GPIOB
#define W25Q64_Flash_MOSI_Pin GPIO_PIN_15
#define W25Q64_Flash_MOSI_GPIO_Port GPIOB
#define COLUMN2_Pin GPIO_PIN_6
#define COLUMN2_GPIO_Port GPIOC
#define COLUMN3_Pin GPIO_PIN_7
#define COLUMN3_GPIO_Port GPIOC
#define AS608__TX_Pin GPIO_PIN_9
#define AS608__TX_GPIO_Port GPIOA
#define AS608__RX_Pin GPIO_PIN_10
#define AS608__RX_GPIO_Port GPIOA
#define OLED_SSD1306_SCL_Pin GPIO_PIN_6
#define OLED_SSD1306_SCL_GPIO_Port GPIOB
#define OLED_SSD1306_SDA_Pin GPIO_PIN_7
#define OLED_SSD1306_SDA_GPIO_Port GPIOB
#define J______________________Pin GPIO_PIN_0
#define J______________________GPIO_Port GPIOE
#define F____________________Pin GPIO_PIN_1
#define F____________________GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
