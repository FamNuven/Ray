/**
  ******************************************************************************
  * @file    timer.h	
  * @brief   
  * @author  nchernov
  * @version 1.0
  * @date    Apr 25, 2024
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
#ifndef TIMER_TIMER_H_
#define TIMER_TIMER_H_
/*----------------------------------------------------------------------------*/
#include "main.h"
#include "structures.h"
/*----------------------------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;        // PWM3
extern TIM_HandleTypeDef htim15;       // PWM4
extern TIM_HandleTypeDef htim16;       // PWM1
extern TIM_HandleTypeDef htim17;       // PWM2
/*----------------------------------------------------------------------------*/
void initTimer(uint8_t numPWM);
void fillingStructures (void);
/*----------------------------------------------------------------------------*/
#endif /* TIMER_TIMER_H_ */
