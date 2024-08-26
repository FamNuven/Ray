/**
  ******************************************************************************
  * @file    rs485.c	
  * @brief   
  * @author  Nikita
  * @version 1.0
  * @date    Jun 19, 2024
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

#include "rs485.h"
#include "usart.h"
#include "DWIN.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief ф-ция переинициализирует RS-485
 */
void reinitRS485 (void)
{
	huart1.Init.BaudRate = setRS485.baud_rate;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_UART_Receive_IT(&huart1, &ring_buffer[0][rx_head[0]], 1);
}


/*----------------------------------------------------------------------------*/
/**
 * @brief ф-ция отправляет данные по RS-485
 * @param data
 * @param lenght
 */
void transmitRS485 (uint8_t *data, uint8_t lenght)
{
	HAL_UART_AbortReceive(&huart1);
	HAL_GPIO_WritePin(GPIOA, DE_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart1, data, lenght, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOA, DE_Pin, GPIO_PIN_RESET);
	clear_buffer(0);
	reinitRS485();                                         // переинициализация после HAL_UART_AbortReceive
	HAL_UART_Receive_IT(&huart1, &ring_buffer[0][rx_head[0]], 1);
}


/*----------------------------------------------------------------------------*/
/**
 * @brief ф-ция обрабатываем принятые данные
 */
void receiveRS485 (void)
{
	HAL_Delay(50);
	uint8_t data_rx[16] = {0,};                            // принимаем по 16 символов
	uint8_t data_tx[42] = {0,};
	uint8_t i = 0;
	while(byte_available(0))                               // читаем все прилетевшие символы (до 16 за раз)
	{
		data_rx[i] = read_byte_from_buffer(0);
		i++;
	}
	data_tx[0] = FRAME_HEADER_H;
	data_tx[1] = FRAME_HEADER_L;
	data_tx[2] = 19;                                       // всегда отправляю полную посылку в 16 символов, чтобы затереть предыдущее сообщение полностью
	data_tx[3] = DWIN_WRITE_VAR;
	data_tx[4] = 0x51;
	data_tx[5] = 0x12;

	for (uint8_t j = 0; j < i; j++)
	{
		data_tx[6+j*2] = (data_rx[j]&0xF0) >> 4;
		data_tx[7+j*2] = data_rx[j]&0x0F;

		data_tx[6+j*2] = int_string(data_tx[6+j*2]);
		data_tx[7+j*2] = int_string(data_tx[7+j*2]);
	}
	HAL_UART_Transmit(&huart3, data_tx, 26, 0xFFF);
}


/*----------------------------------------------------------------------------*/
/**
 * @brief ф-ция переводит цифру в символ
 * @param var
 * @return
 */
uint8_t int_string (uint8_t var)
{
	if (var < 10)
		var += 48;
	else
		var += 87;
	return var;
}
