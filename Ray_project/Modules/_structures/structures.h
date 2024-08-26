/**
  ******************************************************************************
  * @file    structures.h	
  * @brief   
  * @author  nchernov
  * @version 1.0
  * @date    Apr 9, 2024
  ******************************************************************************
  * @attention в этом файле описаны основные структуры проекта
  ******************************************************************************
  */
#ifndef STRUCTURES_STRUCTURES_H_
#define STRUCTURES_STRUCTURES_H_

/*----------------------------------------------------------------------------*/
#include "main.h"

/*----------------------------------------------------------------------------*/
typedef struct
{
	uint32_t ID;                       // addr 5064
	uint32_t baud_rate;                // addr 5080             записывать в килобайт\секунду
} settingsCAN;
extern settingsCAN setCAN;

/*----------------------------------------------------------------------------*/
typedef struct
{
	uint32_t baud_rate;                // addr 5128
} settingsRS485;
extern settingsRS485 setRS485;

/*----------------------------------------------------------------------------*/
typedef struct
{
	uint16_t freq;                     // addr 5000, 5008, 5016, 5024
	uint8_t  duty;                     // addr 5004, 5012, 5020, 5028
	TIM_HandleTypeDef *htim;
} settingsPWM;
extern settingsPWM setPWM[4];

/*----------------------------------------------------------------------------*/
// кольцевой буффер. 0 - USART1 (RS485), 1 - USART3 (DWIN), 2 - CAN
extern uint8_t ring_buffer [3][RX_BUFFER_SIZE];
extern volatile uint8_t rx_head[3];
extern volatile uint8_t rx_tail[3];

/*----------------------------------------------------------------------------*/
#endif /* STRUCTURES_STRUCTURES_H_ */
