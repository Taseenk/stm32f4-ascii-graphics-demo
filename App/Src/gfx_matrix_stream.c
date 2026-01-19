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

	// Determine density based on frame count
	uint32_t spawn_count = (frame % density_scale) + COORDINATE_OFFSET;

	for (int i = 0; i < spawn_count; i++) {
		// Generate random coordinates within terminal bounds
		random_col = (__GetRandomNumber() % TERMINAL_WIDTH) + COORDINATE_OFFSET;
		random_row = (__GetRandomNumber() % TERMINAL_HEIGHT) + COORDINATE_OFFSET;

		// Generate a random character using a bitwisem mask
		// Using 0x3F and adding 33 to stay between a range of printable ASCII character
		char_buffer[0] = (__GetRandomNumber() & ASCII_CHAR_MASK) + ASCII_PRINTABLE_START;

		// Move cursor and draw the character on the terminal
		TerminalSetCursorPos(random_col, random_row);
		SerialPrintN(char_buffer, 1);
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
	// Variables for coordinate tracking
	uint16_t random_col, random_row;

	// Determine density based on frame count
	uint32_t spawn_count = (frame % density_scale) + COORDINATE_OFFSET;

	for (int i = 0; i < spawn_count; i++) {
		// Generate random coordinates within terminal bounds
		random_col = (__GetRandomNumber() % TERMINAL_WIDTH) + COORDINATE_OFFSET;
		random_row = (__GetRandomNumber() % TERMINAL_HEIGHT) + COORDINATE_OFFSET;

		// Move cursor and draw the character on the terminal
		TerminalSetCursorPos(random_col, random_row);
		SerialPrintN(" ", 1);
	}
}
