/**
  ******************************************************************************
  * @file    DWIN.h	
  * @brief   
  * @author  nchernov
  * @version 1.0
  * @date    Jan 12, 2024
  ******************************************************************************
  * @attention
  ******************************************************************************
  */
#ifndef DWIN_DWIN_H_
#define DWIN_DWIN_H_
/*----------------------------------------------------------------------------*/
#include "main.h"
#include "structures.h"
/*----------------------------------------------------------------------------*/
extern UART_HandleTypeDef huart3;
/*----------------------------------------------------------------------------*/
// команды для DWIN
#define DWIN_WRITE_REG     0x80
#define DWIN_READ_REG      0x81
#define DWIN_WRITE_VAR     0x82
#define DWIN_READ_VAR      0x83
#define FRAME_HEADER_H     0x5A
#define FRAME_HEADER_L     0xA5

/*----------------------------------------------------------------------------*/
void init_DWIN (void);
void analysisMessage (void);
void sound_off (void);
uint8_t string_int (uint8_t var);
/*----------------------------------------------------------------------------*/
#endif /* DWIN_DWIN_H_ */
