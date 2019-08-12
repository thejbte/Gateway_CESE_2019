/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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
#define IN4_Pin GPIO_PIN_0
#define IN4_GPIO_Port GPIOB
#define Gpio4_Bus1_Shutdown_Pin GPIO_PIN_6
#define Gpio4_Bus1_Shutdown_GPIO_Port GPIOC
#define Sigfox_Rst_Pin GPIO_PIN_9
#define Sigfox_Rst_GPIO_Port GPIOC
#define IN1_Pin GPIO_PIN_10
#define IN1_GPIO_Port GPIOA
#define IN2_Pin GPIO_PIN_11
#define IN2_GPIO_Port GPIOA
#define IN3_Pin GPIO_PIN_12
#define IN3_GPIO_Port GPIOA
#define Sigfox_shutdown_Pin GPIO_PIN_10
#define Sigfox_shutdown_GPIO_Port GPIOC
#define Gpio2_Bus1_Wakeup_Pin GPIO_PIN_6
#define Gpio2_Bus1_Wakeup_GPIO_Port GPIOB
#define Gpio3_Bus1_Rst_Pin GPIO_PIN_9
#define Gpio3_Bus1_Rst_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
