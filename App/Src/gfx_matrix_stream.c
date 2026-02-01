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
#include <stdint.h>

/* Private Variables ---------------------------------------------------------*/
extern RNG_HandleTypeDef hrng;                                // RNG handle defined in rng.c
static uint16_t matrix_rain_active_col[TERMINAL_WIDTH] = {0}; // Track active character positions for each column

/* Private Function Prototypes -----------------------------------------------*/
static uint32_t __GetRandomNumber(void);
static void __XorshiftRandomNumber(uint32_t *rand_num);

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

/**
 * @fn static void __XorshiftRandomNumber(uint32_t *rand_num)
 * @brief Updates the provided random number using the Xorshift algorithm.
 * @param rand_num Pointer to the random number to be updated.
 */
static void __XorshiftRandomNumber(uint32_t *rand_num)
{
	// Xorshift algorithm to update the random number
	*rand_num ^= *rand_num << XORSHIFT_S1;
	*rand_num ^= *rand_num >> XORSHIFT_S2;
	*rand_num ^= *rand_num << XORSHIFT_S3;
} 

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale)
 * @brief Draws random characters at random positions. Simulating the "Matrix"
 * digital rain effect.
 * @param frame The current frame count used for determining density.
 * @param density_scale The scale factor to adjust the density of characters drawn.
 */
void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale)
{
	// Variables for coordinate tracking and character generation
	uint16_t random_col, random_row;
	char char_buffer[2] = {0, STRING_TERMINATOR};
	uint32_t rand_num = __GetRandomNumber();

	// Determine density based on frame count
	uint32_t spawn_count = (frame % density_scale) + COORDINATE_OFFSET;

	for (int i = 0; i < spawn_count; i++) {
		// Update the random number using Xorshift algorithm
		// Generate random column within terminal bounds
		__XorshiftRandomNumber(&rand_num);
		random_col = (rand_num % TERMINAL_WIDTH) + COORDINATE_OFFSET;

		// Update the random number using Xorshift algorithm
		// Generate random row within terminal bounds
		__XorshiftRandomNumber(&rand_num);
		random_row = (rand_num % TERMINAL_HEIGHT) + COORDINATE_OFFSET;
		
		// Update the random number using Xorshift algorithm
		__XorshiftRandomNumber(&rand_num);
		// Generate a random character using a bitwise mask
		// Using 0x3F and adding 33 to stay between a range of printable ASCII character
		char_buffer[0] = (rand_num & ASCII_CHAR_MASK) + ASCII_PRINTABLE_START;

		// Move cursor and draw the character on the terminal
		TerminalSerialPrintString(char_buffer, random_col, random_row);
	}
}

/**
 * @fn void MatrixCharacterDissolve(uint32_t frame, uint8_t density_scale)
 * @brief Erases random characters at random positions.
 * @param frame The current frame count used for determining density.
 * @param density_scale The scale factor to adjust the density of characters erased.
 */
void MatrixCharacterDissolve(uint32_t frame, uint8_t density_scale)
{
	// Variables for coordinate tracking and character generation
	uint16_t random_col, random_row;
	uint32_t rand_num = __GetRandomNumber();

	// Determine density based on frame count
	uint32_t spawn_count = (frame % density_scale) + COORDINATE_OFFSET;

	for (int i = 0; i < spawn_count; i++) {
		// Update the random number using Xorshift algorithm
		// Generate random column within terminal bounds
		__XorshiftRandomNumber(&rand_num);
		random_col = (rand_num % TERMINAL_WIDTH) + COORDINATE_OFFSET;

		// Update the random number using Xorshift algorithm
		// Generate random row within terminal bounds
		__XorshiftRandomNumber(&rand_num);
		random_row = (rand_num % TERMINAL_HEIGHT) + COORDINATE_OFFSET;

		// Move cursor and erase the character on the terminal
		TerminalSerialPrintString(" ", random_col, random_row);
	}
}

/**
 * @fn void MatrixRainUpdate(uint32_t frame, uint8_t speed, uint8_t density)
 * @brief Updates the "Matrix" rain effect by moving characters down the screen.
 * @param frame The current frame count used for timing the updates.
 * @param density The density factor determining how frequently new characters spawn.
 */
void MatrixRainUpdate(uint32_t frame, uint8_t density)
{
	// Variables for coordinate tracking and character generation
	char char_buffer[2] = {0, STRING_TERMINATOR};
	uint16_t pos, erase_row;
	uint32_t rand_num = __GetRandomNumber();

	// Iterate through every vertical column of the terminal
	for (int i = 0; i <= TERMINAL_WIDTH; i++) {
		// Update the random number using Xorshift algorithm
		__XorshiftRandomNumber(&rand_num);

		// Get the current position of the active character in this column
		pos = matrix_rain_active_col[i];

		// Column is currently active (has a charachter)
		if (pos > COLUMN_INACTIVE) {
			if (pos > TRAIL_LENGTH) {
				// Remove the last charachter of the trail if the position is on the terminal
				erase_row = pos - TRAIL_LENGTH;

				// Only erase if the tail is actually on the screen
				if (erase_row >= COORDINATE_OFFSET && erase_row <= TERMINAL_HEIGHT) {
					TerminalSerialPrintString(" ", i, erase_row);
				}
			}

			// Draw the character at current position if the position is on the terminal
			if (pos <= TERMINAL_HEIGHT) {
				// Generate a random character using a bitwisem mask
				// Using 0x3F and adding 33 to stay between a range of printable ASCII character
				char_buffer[0] = (rand_num & ASCII_CHAR_MASK) + ASCII_PRINTABLE_START;

				// Move cursor and draw the character on the terminal
				TerminalSerialPrintString(char_buffer, i, pos);
			}

			// Increment position for the next frame
			pos++;

			// Set column to inactive if character trail has cleared the screen
			if (pos > TERMINAL_HEIGHT + TRAIL_LENGTH) {
				matrix_rain_active_col[i] = COLUMN_INACTIVE;
			} else {
				matrix_rain_active_col[i] = pos;
			}

		} else {
			// Using bitwise mask determine probability for a new character based on the density
			if (rand_num != FALSE && (rand_num & MATRIX_DENSITY_MASK) < density) {
				// Start a new character at the top to create the raining effect
				matrix_rain_active_col[i] = COORDINATE_OFFSET;
			}
		}
	}
}
