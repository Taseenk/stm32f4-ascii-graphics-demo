/**
 ******************************************************************************
 * @file           : gfx_matrix_stream.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "gfx_matrix_stream.h"
#include "serial_hw.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <string.h>

/* Private Variables ---------------------------------------------------------*/
extern RNG_HandleTypeDef hrng; // RNG handle defined in rng.c

/* Private Function Prototypes -----------------------------------------------*/
static uint32_t __GetRandomNumber(void);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static uint32_t __GetRandomNumber(void)
 * @brief Generates a random number using the HAL RNG peripheral.
 * @return The generated random number, or FALSE if generation failed.
 */
static uint32_t __GetRandomNumber(void)
{
	// Generate a random number using the HAL RNG peripheral
	uint32_t value;

	// Poll the hardware RNG peripheral to Generate a random number
	// Will return the status of HAL RNG GenerateRandomNumber
	HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&hrng, &value);

	// Check if the random number generation was successful
	if (status != HAL_OK) {
		return FALSE;
	}

	// Return the generated random number
	return value;
}

/* Public Functions ----------------------------------------------------------*/
