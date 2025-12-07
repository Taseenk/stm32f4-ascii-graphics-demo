/**
 ******************************************************************************
 * @file           : console.h
 * @brief          :
 ******************************************************************************
 */

#ifndef __CONSOLE_H
#define __CONSOLE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// STM32 specific libraries
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"

// Standard libraries
#include <stddef.h>
#include <string.h>

/* defines -------------------------------------------------------------------*/
#define NEWLINE_STR			"\r\n"	    // Carriage return + New line characters

/* externs -------------------------------------------------------------------*/

/* function prototypes -------------------------------------------------------*/
HAL_StatusTypeDef ConsolePrint(UART_HandleTypeDef *huart, const char *str);
HAL_StatusTypeDef ConsolePrintN(UART_HandleTypeDef *huart, const char *str, size_t len);
HAL_StatusTypeDef ConsolePrintNewLine(UART_HandleTypeDef *huart, const char *str);

#endif /* __CONSOLE_H */
