/**
  ******************************************************************************
  * @file    can.c	
  * @brief   
  * @author  nchernov
  * @version 1.0
  * @date    Mar 20, 2024
  ******************************************************************************
  * @attention эта библиотека содержит функции для работы с CAN
  ******************************************************************************
  */

#include "can.h"
#include "usart.h"
#include "DWIN.h"
#include "rs485.h"
/*----------------------------------------------------------------------------*/
uint8_t ubKeyNumber = 0x0;
FDCAN_RxHeaderTypeDef RxHeader;
FDCAN_TxHeaderTypeDef TxHeader;

/*----------------------------------------------------------------------------*/
/**
 * @brief Функция отправляет сообщение по CAN
 * @param hfdcan - номер CAN
 * @param id - идентификатор посылки
 * @param data - данные для передачи
 * @param lenght - объем данных
 */
void transmitCAN(FDCAN_HandleTypeDef *hfdcan, uint32_t id, uint8_t *data, uint8_t lenght)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    TxHeader.Identifier = id;
    TxHeader.IdType = 0;                         // не использую расширенный идентификатор (EID)
    TxHeader.DataLength = lenght;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, data);
}


/*----------------------------------------------------------------------------*/
/**
 * @brief чтение сообщения из кольцевого буфера (прием CAN)
 */
void receiveCAN(void)
{
	HAL_Delay(50);
	uint8_t data_rx[16] = {0,};                            // принимаем по 16 символов
	uint8_t data_tx[42] = {0,};
	uint8_t i = 0;
	while(byte_available(2))                               // читаем все прилетевшие символы (до 16 за раз)
	{
		data_rx[i] = read_byte_from_buffer(2);
		i++;
	}
	data_tx[0] = FRAME_HEADER_H;
	data_tx[1] = FRAME_HEADER_L;
	data_tx[2] = 19;                                       // всегда отправляю полную посылку в 16 символов, чтобы затереть предыдущее сообщение полностью
	data_tx[3] = DWIN_WRITE_VAR;
	data_tx[4] = 0x50;
	data_tx[5] = 0x48;

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
 * @brief функция переинициализирует FDCAN на новое значение скорости
 */
void reinitFDCAN1(void)
{
	// вычисление предделителя
	uint16_t presc = 0;
	if (setCAN.baud_rate > 0)
		presc = 2000 / setCAN.baud_rate;
	hfdcan1.Init.NominalPrescaler = presc;
	// переинициализация CAN
	HAL_FDCAN_Init(&hfdcan1);
	FDCAN_Config();
}


/*----------------------------------------------------------------------------*/
/**
 * @brief настройка скорости CAN
 */
void FDCAN_Config(void)
{
	FDCAN_FilterTypeDef sFilterConfig;

	// Configure Rx filter
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x0;
	sFilterConfig.FilterID2 = 0x0;
	HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

	// Configure global filter:
	// Filter all remote frames with STD and EXT ID
	// Reject non matching frames with STD ID and EXT ID
	HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

	// Start the FDCAN module
	HAL_FDCAN_Start(&hfdcan1);
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}


/*----------------------------------------------------------------------------*/
/**
 * @brief callback по приему посылки по CAN
 * @param hfdcan
 * @param RxFifo0ITs
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, &ring_buffer[2][rx_head[2]]);
	rx_head[2] += RxHeader.DataLength;
}
