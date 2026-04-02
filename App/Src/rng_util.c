/**
 ******************************************************************************
 * @file           : rng_util.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "rng_util.h"

/* Private Variables ---------------------------------------------------------*/
extern RNG_HandleTypeDef hrng; // RNG handle defined in rng.c

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn static uint32_t GetRandomNumber(void)
 * @brief Generates a random number using the HAL RNG peripheral.
 * @return The generated random number, or FALSE if generation failed.
 */
uint32_t GetRandomNumber(void)
{
	// Generate a random number using the HAL RNG peripheral
	uint32_t value;

	// Poll the hardware RNG peripheral to Generate a random number
	// Will return the status of HAL RNG GenerateRandomNumber
	HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&hrng, &value);

	// Check if the random number generation was successful
	if (status != HAL_OK)
	{
		return FALSE;
	}

	// Return the generated random number
	return value;
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
