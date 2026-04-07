/**
 ******************************************************************************
 * @file           : scene_credits.c
 * @brief          : Implements the Credits scene, a demoscene-style animated
 * credits card modelled after classic ASCII demo group credits screens.
 * Each line slides up from below into its final resting position with a
 * staggered delay, so the header arrives first and the trailing lines follow.
 * Once all lines have settled the layout holds static for the remainder of
 * the scene duration, acting as a natural playlist bookend.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_credits.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <stdio.h>

/* Private Defines -----------------------------------------------------------*/
#define GUTTER_TEXT   "    "                                 // Visual separator between label and value
#define CREDITS_COUNT (sizeof(credits) / sizeof(credits[0])) // Count of the credits array
#define STAGGER_DELAY 10                                     // Frame delay between credit lines

/* Private Variables ---------------------------------------------------------*/

/**
 * @brief Defines the rows for each line of the credits, including headers, spacers, and content lines.
 */
typedef enum {
	// Header: Cast
	LINE_HEADER_CAST = 3,
	LINE_CAST_SPACER,

	LINE_RENDERER,
	LINE_TRANSPORT,
	LINE_TIMING,
	LINE_RANDOM,
	LINE_PALETTE,
	LINE_CREW_GAP,
	LINE_CREW_CAP2,

	// Header: CREW
	LINE_HEADER_CREW = 12,
	LINE_CREW_SPACER,

	LINE_HARDWARE,
	LINE_CORE,
	LINE_CLOCK,
	LINE_MEMORY,

	// Footer
	LINE_FOOTER = 21
} CreditLineRow_t;

/**
 * @brief Structure to define each credit line, including the text to display, the column position, and the target row
 * for animation. The text can include ANSI escape sequences for styling.
 */
typedef struct {
	const char *text;
	uint8_t col;
	CreditLineRow_t target_row;
} CreditLine_t;

// Static array of credit lines with their text, column position, and target row for animation
static const CreditLine_t credits[] = {
    // Header: Cast
    {ANSI_RESET_STYLE "CAST", 38, LINE_HEADER_CAST},
    {ANSI_RESET_STYLE, 1, LINE_CAST_SPACER},

    {ANSI_DIM "RENDERING" ANSI_RESET_STYLE GUTTER_TEXT "ASCII FRAMEBUFFER", 29, LINE_RENDERER},
    {ANSI_DIM "TRANSPORT" ANSI_RESET_STYLE GUTTER_TEXT "UART @ 921600 BAUD", 29, LINE_TRANSPORT},
    {ANSI_DIM "TIMING" ANSI_RESET_STYLE GUTTER_TEXT "ARM SYSTICK", 32, LINE_TIMING},
    {ANSI_DIM "RANDOMNESS" ANSI_RESET_STYLE GUTTER_TEXT "HARDWARE TRNG", 28, LINE_RANDOM},
    {ANSI_DIM "PALETTE" ANSI_RESET_STYLE GUTTER_TEXT "XTERM-256 COLOUR", 31, LINE_PALETTE},
    {ANSI_RESET_STYLE, 1, LINE_CREW_GAP},
    {ANSI_RESET_STYLE, 1, LINE_CREW_CAP2},

    // Header: CREW
    {ANSI_RESET_STYLE "CREW", 38, LINE_HEADER_CREW},
    {ANSI_RESET_STYLE, 1, LINE_CREW_SPACER},

    {ANSI_DIM "HARDWARE" ANSI_RESET_STYLE GUTTER_TEXT "STM32F407VG", 30, LINE_HARDWARE},
    {ANSI_DIM "CORE" ANSI_RESET_STYLE GUTTER_TEXT "ARM CORTEX-M4", 34, LINE_CORE},
    {ANSI_DIM "CLOCK" ANSI_RESET_STYLE GUTTER_TEXT "168 MHZ PLL", 33, LINE_CLOCK},
    {ANSI_DIM "MEMORY" ANSI_RESET_STYLE GUTTER_TEXT "128KB SRAM", 32, LINE_MEMORY},

    // Footer
    {ANSI_RESET_STYLE "(C) 2026 ASCII GRAPHICS DEMO", 27, LINE_FOOTER},
};
static uint8_t current_row[CREDITS_COUNT] = {0}; // Tracks the current animated row position for each credit line

/* Private Function Prototypes -----------------------------------------------*/
static void EraseCreditLine_(uint8_t row);
static void DrawCredits_(uint32_t frame);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void EraseCreditLine_(uint8_t row)
 * @brief Erases a single line of text at the specified row by moving the
 * cursor to the start of the line and sending the ANSI escape sequence to
 * clear the entire line. This is used to create the sliding animation effect
 * as lines move upwards into their final positions.
 * @param row The terminal row index (1-based) where the line should be erased.
 */
static void EraseCreditLine_(uint8_t row)
{
	// Buffer to hold the ANSI escape sequence for erasing a line e.g. ESC[12;1H\x1b[2K
	char buffer[24];

	// Format the escape sequence to move the cursor and return the length
	// The length here is without the string terminator (\0)
	int len = snprintf(buffer, sizeof(buffer), ANSI_ESC "%u;1H\x1b[2K", (unsigned int)row);

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the terminal size
	if (len <= 0 || (size_t)len >= sizeof(buffer))
		return;

	// Send the escape sequence to the terminal to erase the line
	TerminalPrintN(buffer, (uint16_t)len);
}

/**
 * @fn static void DrawCredits_(uint32_t frame)
 * @brief Draws all credit lines at their current animated positions for the
 * given frame. Applies the correct ANSI attribute to each line before
 * printing and resets styling afterwards.
 * @param frame The current frame index provided by the scene manager.
 */
static void DrawCredits_(uint32_t frame)
{
	// Slow down the animation by only updating every 8 frames
	if ((frame % 8) != 0)
		return;

	// Calculate new positions and clear old artifacts
	for (int i = 0; i < CREDITS_COUNT; i++)
	{
		const uint32_t starting_frame = i * STAGGER_DELAY;

		// Skip updating this line until its staggered start time has been reached
		if (frame < starting_frame)
			continue;

		// Animate and render until the line reaches its target row
		if (current_row[i] > credits[i].target_row)
		{
			EraseCreditLine_(current_row[i]);
			current_row[i]--;

			TerminalPrintString(credits[i].text, credits[i].col, current_row[i]);
		}
	}
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void SceneCreditsInit(void)
 * @brief Initializes the credits scene by clearing the terminal and resetting
 * all ANSI styles to a default state ready for the first frame.
 */
void SceneCreditsInit(void)
{
	// Clear the terminal and reset styles to a default state
	TerminalClearAndHome();
	TerminalResetStyle();
	TerminalInvisibleCursor();

	// Initialize all credit lines to start below the visible area of the terminal
	for (int i = 0; i < CREDITS_COUNT; i++)
	{
		current_row[i] = TERMINAL_HEIGHT + 1;
	}
}

/**
 * @fn void SceneCreditsRender(uint32_t scene_frame)
 * @brief
 * @param scene_frame The current frame index provided by the scene manager.
 */
void SceneCreditsRender(uint32_t scene_frame)
{
	DrawCredits_(scene_frame);
}