/**
  ******************************************************************************
  * @file    can.h	
  * @brief   
  * @author  nchernov
  * @version 1.0
  * @date    Mar 20, 2024
  ******************************************************************************
  * @attention эта библиотека содержит функции для работы с CAN
  ******************************************************************************
  */
#ifndef CAN_CAN_H_
#define CAN_CAN_H_
/*----------------------------------------------------------------------------*/
#include "main.h"
#include "structures.h"
/*----------------------------------------------------------------------------*/
extern FDCAN_HandleTypeDef hfdcan1;
/*----------------------------------------------------------------------------*/
void transmitCAN(FDCAN_HandleTypeDef *hfdcan, uint32_t id, uint8_t *data, uint8_t len);
void receiveCAN(void);
void reinitFDCAN1(void);
void FDCAN_Config(void);

/*----------------------------------------------------------------------------*/
#endif /* CAN_CAN_H_ */