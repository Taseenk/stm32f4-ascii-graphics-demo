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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"

#include <stddef.h>
#include <string.h>

/* defines -------------------------------------------------------------------*/
#define NEWLINE_STR			"\r\n"	// Carriage return + New line characters

/* externs -------------------------------------------------------------------*/

/* function prototypes -------------------------------------------------------*/
void consolePrint(UART_HandleTypeDef *huart, const char *str);

#endif /* __CONSOLE_H */
