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
#include "rng_util.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <stdint.h>

/* Private Variables ---------------------------------------------------------*/
static uint16_t matrix_rain_active_col[TERMINAL_WIDTH] = {0}; // Track active character positions for each column

/* Private Function Prototypes -----------------------------------------------*/


/* Private Functions ---------------------------------------------------------*/


/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale)
 * @brief Draws random characters at random positions. Simulating the "Matrix"
 * digital rain effect.
 * @param frame The current frame count used for determining density.
 * @param density_scale The scale factor to adjust the density of characters drawn.
 * @param noise_mode The mode to determine the type of characters drawn (ASCII or Binary).
 */
void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale, uint8_t noise_mode)
{
	// Variables for coordinate tracking and character generation
	uint16_t random_col, random_row;
	char char_buffer[2] = {0, STRING_TERMINATOR};
	uint32_t rand_num = GetRandomNumber();

	// Determine density based on frame count
	uint32_t spawn_count = (frame % density_scale) + COORDINATE_OFFSET;

	for (int i = 0; i < spawn_count; i++) {
		// Update the random number using Xorshift algorithm
		// Generate random column within terminal bounds
		XorshiftRandomNumber(&rand_num);
		random_col = (rand_num % TERMINAL_WIDTH) + COORDINATE_OFFSET;

		// Update the random number using Xorshift algorithm
		// Generate random row within terminal bounds
		XorshiftRandomNumber(&rand_num);
		random_row = (rand_num % TERMINAL_HEIGHT) + COORDINATE_OFFSET;

		// Update the random number using Xorshift algorithm
		XorshiftRandomNumber(&rand_num);

		// Determine the character to draw based on the noise mode
		if (noise_mode == CHARACTER_ASCII_NOISE) {
			// Generate a random character using a bitwise mask
			// Using 0x3F and adding 33 to stay between a range of printable ASCII character
			char_buffer[0] = (rand_num & ASCII_CHAR_MASK) + ASCII_PRINTABLE_START;
		} else if (noise_mode == CHARACTER_BINARY_NOISE) {
			// Generate a random binary character (0 or 1) using a bitwise mask
			char_buffer[0] = (rand_num & BINARY_MASK) + ASCII_ZERO_OFFSET;
		}

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
	uint32_t rand_num = GetRandomNumber();

	// Determine density based on frame count
	uint32_t spawn_count = (frame % density_scale) + COORDINATE_OFFSET;

	for (int i = 0; i < spawn_count; i++) {
		// Update the random number using Xorshift algorithm
		// Generate random column within terminal bounds
		XorshiftRandomNumber(&rand_num);
		random_col = (rand_num % TERMINAL_WIDTH) + COORDINATE_OFFSET;

		// Update the random number using Xorshift algorithm
		// Generate random row within terminal bounds
		XorshiftRandomNumber(&rand_num);
		random_row = (rand_num % TERMINAL_HEIGHT) + COORDINATE_OFFSET;

		// Move cursor and erase the character on the terminal
		TerminalSerialPrintString(" ", random_col, random_row);
	}
}

/**
 * @fn void MatrixRainUpdate(uint32_t frame, uint8_t speed, uint8_t density)
 * @brief Updates the "Matrix" rain effect by moving characters down the screen.
 * @param density The density factor determining how frequently new characters spawn.
 * @param speed The speed factor determining how many rows characters move per update.
 */
void MatrixRainUpdate(uint8_t density, uint8_t speed)
{
	// Variables for coordinate tracking and character generation
	char char_buffer[2] = {0, STRING_TERMINATOR};
	uint16_t pos, erase_row;
	uint32_t rand_num = GetRandomNumber();

	// Iterate through every vertical column of the terminal
	for (int i = 0; i < TERMINAL_WIDTH; i++) {
		// Update the random number using Xorshift algorithm
		XorshiftRandomNumber(&rand_num);

		// Get the current position of the active character in this column
		pos = matrix_rain_active_col[i];

		// Column is currently active (has a charachter)
		if (pos > COLUMN_INACTIVE) {
			// Iterate through the length of trail and erase characters as they move down
			for (uint8_t s = 0; s < speed; s++) {
				// Remove the last charachter of the trail if the position is on the terminal
				erase_row = (pos + s) - TRAIL_LENGTH;

				// Only erase if the tail is actually on the screen
				if (erase_row >= COORDINATE_OFFSET && erase_row <= TERMINAL_HEIGHT) {
					TerminalSerialPrintString(" ", i, erase_row);
				}
			}

			// Iterate through the length of the trail and draw characters as they move down
			for (uint8_t s = 0; s < speed; s++) {
				// Add a character at the current position if it's on the terminal
				uint16_t draw_row = pos + s;

				// Only Add if the position is actually on the screen
				if (draw_row <= TERMINAL_HEIGHT) {
					// Generate a random character using a bitwisem mask
					// Using 0x3F and adding 33 to stay between a range of printable ASCII character
					char_buffer[0] = (rand_num & ASCII_CHAR_MASK) + ASCII_PRINTABLE_START;

					// Move cursor and draw the character on the terminal
					TerminalSerialPrintString(char_buffer, i, draw_row);
				}
			}

			// Increment position for the next frame
			pos += speed;

			// Set column to inactive if character trail has cleared the screen
			if (pos > TERMINAL_HEIGHT + TRAIL_LENGTH) {
				matrix_rain_active_col[i] = COLUMN_INACTIVE;
			} else {
				matrix_rain_active_col[i] = pos;
			}

		} else {
			// Using bitwise mask determine probability for a new character based on the density
			if (rand_num != FALSE && (rand_num & MATRIX_RAIN_DENSITY_MASK) < density) {
				// Start a new character at the top to create the raining effect
				matrix_rain_active_col[i] = COORDINATE_OFFSET;
			}
		}
	}
}
