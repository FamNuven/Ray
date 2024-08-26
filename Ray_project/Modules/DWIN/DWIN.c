/**
  ******************************************************************************
  * @file    DWIN.c	
  * @brief   
  * @author  nchernov
  * @version 1.0
  * @date    Jan 12, 2024
  ******************************************************************************
  * @attention
  * DWIN protocol:
  * пример команды   5A A5 06 83 50 12 01 01 90
  *            byte   0  1  2  3  4  5  6  7  8
  *            0,1 - стандартное начало команды DWIN
  *            2   - количество байт
  *            3   - тип команды (0x82 - запись, 0x83 - чтение)
  *            4,5 - адрес VP, с которым происходит взаимодействие
  *            6   - количество адресов VPs (words) для чтения (два байта)
  *            7.. - данные
  *            Если идет работа со строкой, то по окончанию данных будет еще два или три байта 0xFF
  ******************************************************************************
  */

#include "DWIN.h"
#include "timer.h"
#include "can.h"
#include "rs485.h"
#include "usart.h"
/*----------------------------------------------------------------------------*/
uint8_t data_rx[25];

/*----------------------------------------------------------------------------*/
/**
 * @brief мини инициализация, где ожидается включение дисплея и отключается звук
 */
void init_DWIN (void)
{
	HAL_Delay(1500);                                       // задержка на включение дисплея
	sound_off();
	HAL_Delay(100);                                        // задержка на прием ответа
	clear_buffer(1);                                       // отчистка буффера от прилетевшего ответа
}


/*----------------------------------------------------------------------------*/
/**
 * @brief функция разбирает команды от DWIN
 */
void analysisMessage (void)
{
	data_rx[0] = read_byte_from_buffer(1);
	data_rx[1] = read_byte_from_buffer(1);
	if (data_rx[0] != 0x5a || data_rx[0] != 0xa5)
	{
		// какое-нибудь исправление ошибки или тупо выход из функции
	}
	data_rx[2] = read_byte_from_buffer(1);                 // тут мы получаем количество байт, которые должны быть в сообщение
	while (byte_available(1) < (data_rx[2]));              // в блокировке ждем, когда придет всё сообщение
	for (uint8_t i = 0; i < data_rx[2]; i++)               // считываем все сообщение
		data_rx[3 + i] = read_byte_from_buffer(1);

	// теперь проанализируем посылку
	uint16_t numVP = (data_rx[4] << 8) + data_rx[5];       // вычисляем адрес VP и по нему уже определяем, к чему относиться посылка
	/*----------------------------------------------------------------------------*/
	if (numVP >= 0x5000 &&numVP <= 0x5031)                 // Если команда относится к PWM
	{
		numVP -= 0x5000;
		uint8_t i = numVP % 8;
		if (i == 0)                                        // пришло значение частоты
			setPWM[numVP / 8].freq = (data_rx[7] << 8) + data_rx[8];
		else                                               // пришло значение скважности
			setPWM[(numVP - 4) / 8].duty = (data_rx[7] << 8) + data_rx[8];
		// Инициализация таймеров
		if ((numVP == 0)||(numVP == 4))
			initTimer(0);
		else if ((numVP == 8)||(numVP == 12))
			initTimer(1);
		else if ((numVP == 16)||(numVP == 20))
			initTimer(2);
		else if ((numVP == 24)||(numVP == 28))
			initTimer(3);
	}
	/*----------------------------------------------------------------------------*/
	else if (numVP == 0x5032)                              // передача по CAN
	{
		if (setCAN.baud_rate > 0)                          // если скорость CAN была настроена, то можно отправлять
		{
			uint8_t can_transmit[16] = {0,};               // массив для хранения преобразованных данных
			uint8_t lenght = 0;
			while (data_rx[lenght + 7] != 0xFF)            // пока не найдем конец строки
			{
				can_transmit[lenght] = string_int (data_rx[lenght + 7]);
				lenght++;
			}
			for (uint8_t i = 0; i <= lenght; i++)
			{
				can_transmit[i] = (can_transmit[2*i] << 4) + can_transmit[2*i+1];
			}
			transmitCAN(&hfdcan1, setCAN.ID, can_transmit, (lenght+1)/2);
		}
	}
	else if (numVP == 0x5064)                              // CAN ID init
	{
		data_rx[7] = string_int(data_rx[7]);
		data_rx[8] = string_int(data_rx[8]);
		setCAN.ID = (data_rx[7] << 4) + data_rx[8];
	}
	else if (numVP == 0x5080)                              // CAN baudrate init
	{
		setCAN.baud_rate = (data_rx[7] << 24) + (data_rx[8] << 16) + (data_rx[9] << 8) + data_rx[10];
		reinitFDCAN1();
	}
	/*----------------------------------------------------------------------------*/
	else if (numVP == 0x5096)                              // передача по RS485
	{
		if (setRS485.baud_rate > 0)
		{
			uint8_t rs_transmit[16] = {0,};                // массив для хранения преобразованных данных
			uint8_t lenght = 0;
			while (data_rx[lenght + 7] != 0xFF)            // пока не найдем конец строки
			{
				rs_transmit[lenght] = string_int (data_rx[lenght + 7]);
				lenght++;
			}
			// получили массив rs_transmit, в котором каждый символ преоразован в hex и занимает целый байт вместо 4 бит
			// сжимаем полученный массив. В каждом байте должно быть по два значения
			for (uint8_t i = 0; i <= lenght; i++)
			{
				rs_transmit[i] = (rs_transmit[2*i] << 4) + rs_transmit[2*i+1];
			}
			transmitRS485(rs_transmit, (lenght+1)/2);
		}
	}
	else if (numVP == 0x5128)                              // CAN baudrate init
	{
		setRS485.baud_rate = (data_rx[7] << 24) + (data_rx[8] << 16) + (data_rx[9] << 8) + data_rx[10];
		reinitRS485();
	}
	/*----------------------------------------------------------------------------*/
	// clear buffer
	for(uint8_t i = 0; i < 26; i++)
		data_rx[i] = 0;
}


/*----------------------------------------------------------------------------*/
/**
 * @brief отправка команды для отключения звука
 */
void sound_off (void)
{
	uint8_t data_tx[10];
	data_tx[0] = FRAME_HEADER_H;
	data_tx[1] = FRAME_HEADER_L;
	data_tx[2] = 0x07;
	data_tx[3] = DWIN_WRITE_VAR;
	data_tx[4] = 0x00;
	data_tx[5] = 0x80;
	data_tx[6] = 0x5A;
	data_tx[7] = 0x00;
	data_tx[8] = 0x00;
	data_tx[9] = 0x30;
	HAL_UART_Transmit(&huart3, data_tx, 10, 0xFFF);
}


/*----------------------------------------------------------------------------*/
/**
 * @brief функция преобразует символ в число (от 0 до F)
 * @param var переменная для преобразования
 * @return преобразованное число
 */
uint8_t string_int (uint8_t var)
{
	if (var < 59)                     // символы от 0 до 9
		var -= 48;
	else
		var -= 87;                    // перевод маленьких букв a,b,c,d в значения 10,11,12
	return var;
}



