/*
 * Event.c
 *
 *  Created on: 1/02/2019
 *      Author: julian
 */

#include <Common/Event.h>


void HAL_SYSTICK_Callback(void)
{
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SYSTICK_Callback could be implemented in the user file
           	   Se debe incluir en stm32l0xx_it  HAL_SYSTICK_IRQHandler
   */
	//Debounce_Update(&DebounceData, HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));

}
