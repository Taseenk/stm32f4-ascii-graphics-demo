/**
 ******************************************************************************
 * @file           : rng_util.c
 * @brief          : Hardware Random Number Generator (RNG) utility functions.
 * This module provides abstraction and helper functions for generating random
 * numbers utilizing the hardware RNG peripheral on the STM32F4 microcontroller.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "rng_util.h"
#include "main.h"

/* Private Variables ---------------------------------------------------------*/
extern RNG_HandleTypeDef hrng; // RNG handle defined in rng.c

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn uint8_t GetRandomNumber(uint32_t *out_value)
 * @brief Generates a random number using the HAL RNG peripheral and stores it in the provided output variable.
 * @param value Pointer to a variable where the generated random number will be stored.
 * @return TRUE if the random number was successfully generated, FALSE otherwise.
 */
uint8_t GetRandomNumber(uint32_t *value)
{
	// Check if the output pointer is valid
	if (value == NULL)
		return FALSE;

	// Poll the hardware RNG peripheral to Generate a random number
	// Will return the status of HAL RNG GenerateRandomNumber
	HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&hrng, value);

	// Check if the random number generation was successful
	if (status != HAL_OK)
		return FALSE;

	// Return true if the random number was successfully generated
	return TRUE;
}

/**
 * @fn static void XorshiftRandomNumber(uint32_t *rand_num)
 * @brief Updates the provided random number using the Xorshift algorithm.
 * @param rand_num Pointer to the random number to be updated.
 */
void XorshiftRandomNumber(uint32_t *rand_num)
{
	// Xorshift algorithm to update the random number
	*rand_num ^= *rand_num << XORSHIFT_S1;
	*rand_num ^= *rand_num >> XORSHIFT_S2;
	*rand_num ^= *rand_num << XORSHIFT_S3;
}
