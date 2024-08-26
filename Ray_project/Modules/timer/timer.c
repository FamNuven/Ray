/**
  ******************************************************************************
  * @file    timer.c	
  * @brief   
  * @author  nchernov
  * @version 1.0
  * @date    Apr 25, 2024
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

#include "timer.h"

/*----------------------------------------------------------------------------*/
/**
 * @brief функция переинициализирует таймер на новые значения частоты и скважности
 * @param numPWM принимает номер шима
 */
void initTimer(uint8_t numPWM)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	// пересчет значения предделителя
	if (setPWM[numPWM].freq > 0 && setPWM[numPWM].duty > 0)
	{
		setPWM[numPWM].htim->Init.Prescaler = 320000 / setPWM[numPWM].freq - 1;  // 128 MHz/100 (100 is period)
		setPWM[numPWM].htim->Init.Period = 99;

		HAL_TIM_PWM_Init(setPWM[numPWM].htim);
		// инициализация каналов
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = setPWM[numPWM].duty;
		if (numPWM < 2)
		{
			HAL_TIM_PWM_ConfigChannel(setPWM[numPWM].htim, &sConfigOC, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(setPWM[numPWM].htim, TIM_CHANNEL_1);
		}
		else if (numPWM == 2)
		{
			HAL_TIM_PWM_ConfigChannel(setPWM[numPWM].htim, &sConfigOC, TIM_CHANNEL_3);
			HAL_TIM_PWM_Start(setPWM[numPWM].htim, TIM_CHANNEL_3);
		}

		else if (numPWM == 3)
		{
			HAL_TIM_PWM_ConfigChannel(setPWM[numPWM].htim, &sConfigOC, TIM_CHANNEL_2);
			HAL_TIM_PWM_Start(setPWM[numPWM].htim, TIM_CHANNEL_2);
		}
	}
	// если таймер надо отключить (частота = 0)
	else
	{
		if (numPWM < 2)
			HAL_TIM_PWM_Stop(setPWM[numPWM].htim, TIM_CHANNEL_1);
		else if (numPWM == 2)
			HAL_TIM_PWM_Stop(setPWM[numPWM].htim, TIM_CHANNEL_3);
		else if (numPWM == 3)
			HAL_TIM_PWM_Stop(setPWM[numPWM].htim, TIM_CHANNEL_2);
	}
}


/*----------------------------------------------------------------------------*/
/**
 * @brief функция записывает значения соответсвующих таймеров в структуру
 */
void fillingStructures (void)
{
	setPWM[0].htim = &htim16;
	setPWM[1].htim = &htim17;
	setPWM[2].htim = &htim3;
	setPWM[3].htim = &htim15;
}
