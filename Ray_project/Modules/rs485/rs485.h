/**
  ******************************************************************************
  * @file    rs485.h	
  * @brief   
  * @author  Nikita
  * @version 1.0
  * @date    Jun 19, 2024
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
#ifndef RS485_RS485_H_
#define RS485_RS485_H_
/*----------------------------------------------------------------------------*/
#include "main.h"
#include "structures.h"

/*----------------------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/*----------------------------------------------------------------------------*/
void reinitRS485 (void);
void transmitRS485 (uint8_t *data, uint8_t lenght);
void receiveRS485 (void);
uint8_t int_string (uint8_t var);
#endif /* RS485_RS485_H_ */