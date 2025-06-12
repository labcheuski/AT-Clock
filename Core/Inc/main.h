/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED13_Pin GPIO_PIN_13
#define LED13_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_1
#define BUZZER_GPIO_Port GPIOA
#define ST7789_DC_Pin GPIO_PIN_3
#define ST7789_DC_GPIO_Port GPIOA
#define RADIO_D0_Pin GPIO_PIN_4
#define RADIO_D0_GPIO_Port GPIOA
#define RADIO_D1_Pin GPIO_PIN_5
#define RADIO_D1_GPIO_Port GPIOA
#define RADIO_D2_Pin GPIO_PIN_6
#define RADIO_D2_GPIO_Port GPIOA
#define RADIO_D3_Pin GPIO_PIN_7
#define RADIO_D3_GPIO_Port GPIOA
#define ST7789_RST_Pin GPIO_PIN_0
#define ST7789_RST_GPIO_Port GPIOB
#define BUT_MODE_Pin GPIO_PIN_1
#define BUT_MODE_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOB
#define BUT_DOWN_Pin GPIO_PIN_10
#define BUT_DOWN_GPIO_Port GPIOB
#define BUT_UP_Pin GPIO_PIN_11
#define BUT_UP_GPIO_Port GPIOB
#define P10_S_Pin GPIO_PIN_13
#define P10_S_GPIO_Port GPIOB
#define P10_R_Pin GPIO_PIN_15
#define P10_R_GPIO_Port GPIOB
#define P10_A_Pin GPIO_PIN_8
#define P10_A_GPIO_Port GPIOA
#define P10_B_Pin GPIO_PIN_9
#define P10_B_GPIO_Port GPIOA
#define P10_L_Pin GPIO_PIN_10
#define P10_L_GPIO_Port GPIOA
#define P10_OE_Pin GPIO_PIN_11
#define P10_OE_GPIO_Port GPIOA
#define BUTTON_Pin GPIO_PIN_15
#define BUTTON_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOB
#define DS3231_32K_Pin GPIO_PIN_4
#define DS3231_32K_GPIO_Port GPIOB
#define DS3231_SQW_Pin GPIO_PIN_5
#define DS3231_SQW_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
