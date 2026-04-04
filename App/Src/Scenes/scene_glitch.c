/**
 ******************************************************************************
 * @file           : scene_glitch.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_glitch.h"
#include "rng_util.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

/* Private Defines -----------------------------------------------------------*/
#define COORDINATE_OFFSET     1 // Terminal coordinates usually start at 1,1

#define BINARY_CHARACTER      1 // Binary noise character mode
#define ASCII_CHARACTER       0 // ASCII noise character mode

#define BINARY_MASK           1    // Mask for binary character generation
#define ASCII_PRINTABLE_START 33   // The '!' character
#define ASCII_CHAR_MASK       0x3F // Mask to get 64 printable ASCII characters
#define ASCII_ZERO_OFFSET     48   // Offset for ASCII code '0' character

// Scene configuration
#define GLITCH_SCENE_START     0  // Scene frame index to start full brightness
#define GLITCH_SCENE_BRIGHT    21 // Scene frame index to start medium green
#define GLITCH_SCENE_DIM       75 // Scene frame index to start dark green

#define GLITCH_DENSITY_HIGH    20 // Max character spawn rate
#define GLITCH_DENSITY_MEDIUM  4  // Reduced character spawn rate
#define GLITCH_DENSITY_LOW     2  // Low character spawn rate

#define DISSOLVE_INTERVAL_MASK 0x04 // Used for bitwise frame frequency (frame & 4)
#define DISSOLVE_STRENGTH_LOW  5    // Characters removed per frame (Subtle dissolve)
#define DISSOLVE_STRENGTH_HIGH 15   // Characters removed per frame (Aggressive dissolve)

/* Private Variables ---------------------------------------------------------*/
static uint32_t rand_number; // Global random number state for consistent RNG across functions

/* Private Function Prototypes -----------------------------------------------*/
static char GetAsciiOrBinaryChar_(uint8_t noise_mode);
static void CharacterNoise_(uint32_t frame, uint8_t density_scale, uint8_t noise_mode);
static void CharacterDissolve_(uint32_t frame, uint8_t density_scale);
static void RenderGlitchFrame_(uint32_t scene_frame, uint8_t noise_mode);

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
 * @fn static void CharacterNoise_(uint32_t frame, uint8_t density_scale, uint8_t noise_mode)
 * @brief Generates random characters at random positions.
 * @param frame The current frame count used for determining density.
 * @param density_scale The scale factor to adjust the density of characters generated.
 * @param noise_mode The mode for generating noise characters (binary or ASCII).
 */
static void CharacterNoise_(uint32_t frame, uint8_t density_scale, uint8_t noise_mode)
{
	// Variables for coordinate tracking and character generation
	uint16_t random_col, random_row;
	char char_buffer[2] = {0, '\0'};

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

		// Update the random number using Xorshift algorithm
		XorshiftRandomNumber(&rand_number);

		// Determine the character to draw based on the noise mode
		if (noise_mode == ASCII_CHARACTER)
		{
			char_buffer[0] = GetAsciiOrBinaryChar_(ASCII_CHARACTER);
		} else if (noise_mode == BINARY_CHARACTER)
		{
			char_buffer[0] = GetAsciiOrBinaryChar_(BINARY_CHARACTER);
		}

		// Move cursor and draw the character on the terminal
		TerminalPrintString(char_buffer, random_col, random_row);
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
 * @fn static void RenderGlitchFrame_(uint32_t scene_frame, uint8_t noise_mode)
 * @brief Renders a single frame of the glitch effect based on the current scene frame and noise mode.
 * It adjusts the text color and character density to create a dynamic glitch effect.
 * @param scene_frame The current frame index provided by the scene manager.
 * @param noise_mode The mode for generating noise characters (binary or ASCII).
 */
static void RenderGlitchFrame_(uint32_t scene_frame, uint8_t noise_mode)
{
	// Adjust text colour based on time in scene to create a dynamic visual effect
	if (scene_frame == GLITCH_SCENE_START)
		TerminalSetTextColour(FG_BRIGHT_GREEN);
	else if (scene_frame == GLITCH_SCENE_BRIGHT)
		TerminalSetTextColour(FG_MEDIUM_GREEN);
	else if (scene_frame == GLITCH_SCENE_DIM)
		TerminalSetTextColour(FG_DARK_GREEN);

	// Full Brightness
	if (scene_frame < GLITCH_SCENE_BRIGHT)
	{
		// Spawn random characters
		CharacterNoise_(scene_frame, GLITCH_DENSITY_HIGH, noise_mode);

		// Occasional light dissolving to keep the screen from getting too crowded
		if ((scene_frame & DISSOLVE_INTERVAL_MASK) == 0)
			CharacterDissolve_(scene_frame, DISSOLVE_STRENGTH_LOW);
	}

	// Light Dimming
	else if (scene_frame < GLITCH_SCENE_DIM)
	{
		// Spawn fewer new characters, dissolve more existing ones
		CharacterNoise_(scene_frame, GLITCH_DENSITY_MEDIUM, noise_mode);
		CharacterDissolve_(scene_frame, DISSOLVE_STRENGTH_HIGH);
	}

	// Deeper Fade
	else
	{
		// Very few new characters and keep dissolving
		CharacterNoise_(scene_frame, GLITCH_DENSITY_LOW, noise_mode);
		CharacterDissolve_(scene_frame, DISSOLVE_STRENGTH_HIGH);
	}
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void GlitchInit(void)
 * @brief Initializes the glitch scene by setting up necessary variables and state.
 */
void GlitchInit(void)
{
	// Initialize by generating a random number to minimise overhead when the scene starts
	rand_number = GetRandomNumber();

	// Reset all previous styles
	TerminalResetStyle();
}

/**
 * @fn void AsciiGlitchRender(uint32_t scene_frame)
 * @brief Renders a single frame of the ASCII noise glitch effect.
 * @param scene_frame The current frame index provided by the scene manager.
 */
void AsciiGlitchRender(uint32_t scene_frame)
{
	// Render the glitch frame using ascii character mode
	RenderGlitchFrame_(scene_frame, ASCII_CHARACTER);
}

/**
 * @fn void BinaryGlitchRender(uint32_t scene_frame)
 * @brief Renders a single frame of the Binary (0/1) noise glitch effect.
 * @param scene_frame The current frame index provided by the scene manager.
 */
void BinaryGlitchRender(uint32_t scene_frame)
{
	// Render the glitch frame using binary character mode
	RenderGlitchFrame_(scene_frame, BINARY_CHARACTER);
}