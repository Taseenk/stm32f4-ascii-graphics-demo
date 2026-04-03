/**
 ******************************************************************************
 * @file           : scene_matrix_rain.c
 * @brief          : Implements the classic "Matrix Rain" visual effects.
 * Includes variations for ASCII, Binary, "Hacked" (red)
 * corruption, and smooth fade-in transitions.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_matrix_rain.h"
#include "rng_util.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

/* Private Defines -----------------------------------------------------------*/
// Animation Behavior & Timing
#define COORDINATE_OFFSET        1     // Terminal coordinates usually start at 1,1
#define TRAIL_LENGTH             8     // Default trail length for matrix rain effect
#define MATRIX_RAIN_DENSITY_MASK 0x3FF // Mask to normalize RNG for density comparison (0-1023) in matrix rain
#define COLUMN_INACTIVE          0     // Indicator for inactive column

#define BINARY_CHARACTER         1 // Binary character mode
#define ASCII_CHARACTER          0 // ASCII character mode

#define BINARY_MASK              1    // Mask for binary character generation
#define ASCII_PRINTABLE_START    33   // The '!' character
#define ASCII_CHAR_MASK          0x3F // Mask to get 64 printable ASCII characters
#define ASCII_ZERO_OFFSET        48   // Offset for ASCII code '0' character

// Matrix falling Rain Scene
#define RAIN_COLOR_STAGES  3  // Number of color stages for a dynamic visual effect
#define RAIN_DENSITY_HIGH  13 // Most dense rain trail spawn rate
#define RAIN_DENSITY_MID   6  // Medium rain trail spawn rate
#define RAIN_DENSITY_LOW   3  // Sparse rain trail spawn rate

#define RAIN_SPEED_DEFAULT 1 // Number of rows characters move per update at low speed

// Hacked Falling rain scene
#define RAIN_HACKED_CORRUPTED 265 // Scene frame index when the "hack" takes full effect and characters become red
#define RAIN_HACKED_CORRUPT_MASK                                                                                       \
	0x3ff // Bitwise mask to create a small chance of red characters appearing (getting hacked)

// Rain Fade-In Scene
#define RAIN_FADE_IN_SCENE_BRIGHT 40
#define RAIN_FADE_IN_SCENE_MEDIUM 80
#define RAIN_FADE_IN_SCENE_DARK   160

#define RAIN_FADE_IN_DURATION     300                         // Duration of the rain fade-in scene in frames
#define RAIN_FADE_IN_MIDPOINT     (RAIN_FADE_IN_DURATION / 2) // Mid-point for density swap

// Dissolve configuration
#define DISSOLVE_INTERVAL_MASK 0x04 // Used for bitwise frame frequency (frame & 4)
#define DISSOLVE_STRENGTH_LOW  5    // Characters removed per frame (Subtle dissolve)
#define DISSOLVE_STRENGTH_HIGH 15   // Characters removed per frame (Aggressive dissolve)

/* Private Variables ---------------------------------------------------------*/
static uint32_t rand_number; // Global random number state for consistent RNG across functions
static uint16_t matrix_rain_active_col[TERMINAL_WIDTH] = {0}; // Track active character positions for each column

/* Private Function Prototypes -----------------------------------------------*/
static char GetAsciiOrBinaryChar_(uint8_t noise_mode);
static void RainUpdater_(uint8_t density, uint8_t speed, uint8_t noise_mode);
static void CharacterDissolve_(uint32_t frame, uint8_t density_scale);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static char GetAsciiOrBinaryChar_(uint8_t noise_mode)
 * @brief Returns a random printable character based on the noise mode.
 * ASCII mode returns a random printable character in the range 33-96.
 * Binary mode returns either '0' or '1'.
 * @param noise_mode The character mode to use, either CHARACTER_ASCII_NOISE
 * or CHARACTER_BINARY_NOISE.
 * @return A single character determined by the noise mode.
 */
static char GetAsciiOrBinaryChar_(uint8_t noise_mode)
{
	if (noise_mode == BINARY_CHARACTER)
	{
		// Return a random binary character (0 or 1) using a bitwise mask
		return (rand_number & BINARY_MASK) + ASCII_ZERO_OFFSET;
	}

	// Return a random character using a bitwise mask
	// Using 0x3F and adding 33 to stay between a range of printable ASCII character
	return (rand_number & ASCII_CHAR_MASK) + ASCII_PRINTABLE_START;
}

/**
 * @fn static void RainUpdater_(uint8_t density, uint8_t speed, uint8_t noise_mode)
 * @brief Updates the "Matrix" rain effect by moving characters down the screen.
 * @param density The density factor determining how frequently new characters spawn.
 * @param speed The speed factor determining how many rows characters move per update.
 * @param noise_mode The mode for generating noise characters (binary or ASCII).
 */
static void RainUpdater_(uint8_t density, uint8_t speed, uint8_t noise_mode)
{
	// Variables for coordinate tracking and character generation
	uint16_t pos, erase_row;
	char char_buffer[2] = {0, '\0'};

	// Iterate through every vertical column of the terminal
	for (int i = 0; i < TERMINAL_WIDTH; i++)
	{
		// Update the random number using Xorshift algorithm
		XorshiftRandomNumber(&rand_number);

		// Get the current position of the active character in this column
		pos = matrix_rain_active_col[i];

		// Column is currently active (has a charachter)
		if (pos > COLUMN_INACTIVE)
		{
			// Iterate through the length of trail and erase characters as they move down
			for (uint8_t s = 0; s < speed; s++)
			{
				// Remove the last charachter of the trail if the position is on the terminal
				erase_row = (pos + s) - TRAIL_LENGTH;

				// Only erase if the tail is actually on the screen
				if (erase_row >= COORDINATE_OFFSET && erase_row <= TERMINAL_HEIGHT)
				{
					TerminalPrintString(" ", i, erase_row);
				}
			}

			// Iterate through the length of the trail and draw characters as they move down
			for (uint8_t s = 0; s < speed; s++)
			{
				// Add a character at the current position if it's on the terminal
				uint16_t draw_row = pos + s;

				// Only Add if the position is actually on the screen
				if (draw_row <= TERMINAL_HEIGHT)
				{
					// Determine the character to draw based on the noise mode
					if (noise_mode == ASCII_CHARACTER)
					{
						char_buffer[0] = GetAsciiOrBinaryChar_(ASCII_CHARACTER);
					} else if (noise_mode == BINARY_CHARACTER)
					{
						char_buffer[0] = GetAsciiOrBinaryChar_(BINARY_CHARACTER);
					}

					// Move cursor and draw the character on the terminal
					TerminalPrintString(char_buffer, i, draw_row);
				}
			}

			// Increment position for the next frame
			pos += speed;

			// Set column to inactive if character trail has cleared the screen
			if (pos > TERMINAL_HEIGHT + TRAIL_LENGTH)
			{
				matrix_rain_active_col[i] = COLUMN_INACTIVE;
			} else
			{
				matrix_rain_active_col[i] = pos;
			}

		} else
		{
			// Using bitwise mask determine probability for a new character based on the density
			if (rand_number != FALSE && (rand_number & MATRIX_RAIN_DENSITY_MASK) < density)
			{
				// Start a new character at the top to create the raining effect
				matrix_rain_active_col[i] = COORDINATE_OFFSET;
			}
		}
	}
}

/**
 * @fn static void CharacterDissolve_(uint32_t frame, uint8_t density_scale)
 * @brief Erases random characters at random positions.
 * @param frame The current frame count used for determining density.
 * @param density_scale The scale factor to adjust the density of characters erased.
 */
static void CharacterDissolve_(uint32_t frame, uint8_t density_scale)
{
	// Variables for coordinate tracking and character generation
	uint16_t random_col, random_row;

	// Determine density based on frame count
	uint32_t spawn_count = (frame % density_scale) + COORDINATE_OFFSET;

	for (int i = 0; i < spawn_count; i++)
	{
		// Update the random number using Xorshift algorithm
		// Generate random column within terminal bounds
		XorshiftRandomNumber(&rand_number);
		random_col = (rand_number % TERMINAL_WIDTH) + COORDINATE_OFFSET;

		// Update the random number using Xorshift algorithm
		// Generate random row within terminal bounds
		XorshiftRandomNumber(&rand_number);
		random_row = (rand_number % TERMINAL_HEIGHT) + COORDINATE_OFFSET;

		// Move cursor and erase the character on the terminal
		TerminalPrintString(" ", random_col, random_row);
	}
}

/**
 * @fn static void RenderMatrixRainFrame_(uint32_t scene_frame, uint8_t noise_mode)
 * @brief Renders a single frame of the matrix rain effect based on the current scene frame and noise mode.
 * It cycles through text colors to create a dynamic visual effect and updates the rain trails.
 * It also applies occasional dissolving to prevent overcrowding of characters on the screen.
 * @param scene_frame The current frame index provided by the scene manager.
 * @param noise_mode The mode for generating noise characters (binary or ASCII).
 */
static void RenderMatrixRainFrame_(uint32_t scene_frame, uint8_t noise_mode)
{
	// Cycle through text colours to create a dynamic visual effect
	uint32_t color_cycle = (scene_frame / TERMINAL_HEIGHT) % RAIN_COLOR_STAGES;
	if (color_cycle == 0)
		TerminalSetTextColour(FG_BRIGHT_GREEN);
	else if (color_cycle == 1)
		TerminalSetTextColour(FG_MEDIUM_GREEN);
	else
		TerminalSetTextColour(FG_DARK_GREEN);

	// Create falling rain trails
	RainUpdater_(RAIN_DENSITY_HIGH, RAIN_SPEED_DEFAULT, noise_mode);

	// Occasional light dissolving to keep the screen from getting too crowded
	if ((scene_frame & DISSOLVE_INTERVAL_MASK) == 0)
		CharacterDissolve_(scene_frame, DISSOLVE_STRENGTH_LOW);
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void MatrixRainInit(void)
 * @brief Prepares the matrix rain state by setting up necessary variables and state.
 */
void MatrixRainInit(void)
{
	// Initialize by generating a random number to minimise overhead when the scene starts
	rand_number = GetRandomNumber();

	// Reset all previous styles
	TerminalResetStyle();
}

/**
 * @fn void AsciiRainRender(uint32_t scene_frame)
 * @brief Renders a frame of the standard ASCII matrix rain.
 * @param scene_frame The current frame index from the scene manager.
 */
void AsciiRainRender(uint32_t scene_frame)
{
	// Render the matrix rain frame using ascii character mode
	RenderMatrixRainFrame_(scene_frame, ASCII_CHARACTER);
}

/**
 * @fn void BinaryRainRender(uint32_t scene_frame)
 * @brief Renders a frame of the binary (0 and 1) matrix rain.
 * @param scene_frame The current frame index from the scene manager.
 */
void BinaryRainRender(uint32_t scene_frame)
{
	// Render the matrix rain frame using binary character mode
	RenderMatrixRainFrame_(scene_frame, BINARY_CHARACTER);
}

/**
 * @fn void AsciiRainHackedRender(uint32_t scene_frame)
 * @brief Renders the "Hacked" rain effect where characters turn red over time.
 * @param scene_frame The current frame index from the scene manager.
 */
void AsciiRainHackedRender(uint32_t scene_frame)
{
	uint32_t color_cycle = (scene_frame / TERMINAL_HEIGHT) % RAIN_COLOR_STAGES;

	// System has been hacked/taken over data is corrupt (text becomes red)
	if (scene_frame > RAIN_HACKED_CORRUPTED)
	{
		// Cycle through red tones instead of green
		if (color_cycle == 0)
			TerminalSetTextColour(FG_BRIGHT_RED);
		else if (color_cycle == 1)
			TerminalSetTextColour(FG_MEDIUM_RED);
		else
			TerminalSetTextColour(FG_DARK_RED);
	} else
	{
		// Update the random number using Xorshift algorithm
		XorshiftRandomNumber(&rand_number);

		// Bitwise mask to create a small chance of red characters appearing (getting hacked)
		if ((rand_number & RAIN_HACKED_CORRUPT_MASK) < 7)
		{
			TerminalSetTextColour(FG_MEDIUM_RED);
		} else
		{
			// Initial normal green fade-in of the rain before the "hack" takes over
			if (color_cycle == 0)
			{
				TerminalSetTextColour(FG_BRIGHT_GREEN);
			} else if (color_cycle == 1)
				TerminalSetTextColour(FG_MEDIUM_GREEN);
			else
				TerminalSetTextColour(FG_DARK_GREEN);
		}
	}

	// Create falling rain trails
	RainUpdater_(RAIN_DENSITY_HIGH, RAIN_SPEED_DEFAULT, ASCII_CHARACTER);

	// Occasional light dissolving to keep the screen from getting too crowded
	if ((scene_frame & DISSOLVE_INTERVAL_MASK) == 0)
		CharacterDissolve_(scene_frame, DISSOLVE_STRENGTH_LOW);
}

/**
 * @fn void AsciiRainFadeIn(uint32_t scene_frame)
 * @brief Renders a sequence where the rain starts sparse/bright and becomes dense/dark.
 * Used for transitioning into the full matrix rain effect with a fade-in style.
 * @param scene_frame The current frame index from the scene manager.
 */
void AsciiRainFadeIn(uint32_t scene_frame)
{
	// Adjust text colour based on time in scene
	if (scene_frame < RAIN_FADE_IN_SCENE_BRIGHT)
	{
		TerminalSetTextColour(FG_BRIGHT_GREEN);
	} else if (scene_frame < RAIN_FADE_IN_SCENE_MEDIUM)
	{
		TerminalSetTextColour(FG_MEDIUM_GREEN);
	} else if (scene_frame < RAIN_FADE_IN_SCENE_DARK)
	{
		TerminalSetTextColour(FG_DARK_GREEN);
	}

	// Spawn rain trails with increasing density as the scene progresses
	if (scene_frame < RAIN_FADE_IN_MIDPOINT)
		RainUpdater_(RAIN_DENSITY_LOW, RAIN_SPEED_DEFAULT, ASCII_CHARACTER);
	else
		RainUpdater_(RAIN_DENSITY_HIGH, RAIN_SPEED_DEFAULT, ASCII_CHARACTER);

	// Disolving charachters at intervals
	if (scene_frame % DISSOLVE_INTERVAL_MASK == 0)
		CharacterDissolve_(scene_frame, DISSOLVE_STRENGTH_HIGH);
}