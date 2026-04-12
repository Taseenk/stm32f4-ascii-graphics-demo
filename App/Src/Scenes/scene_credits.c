/**
 ******************************************************************************
 * @file           : scene_credits.c
 * @brief          : Implements the Credits scene, a vertical credit crawl inspired
 * by movie end sequences. Text lines enter from the bottom edge and move toward their
 * assigned layout positions. The timing uses a staggered entry system so that
 * headers lead the crawl, followed by the supporting cast and crew,
 * eventually locking into a final static billboard.
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
#define STAGGER_DELAY 15                                     // Frame delay between lines starts

// Scene timing defines (in frames)
#define SLIDE_IN_END 250                   // frames for all lines to slide in and settle
#define HOLD_END     270                   // frames to stay static
#define TITLE_TARGET (TERMINAL_HEIGHT / 2) // centre row for the footer title

/* Private Variables ---------------------------------------------------------*/

/**
 * @brief Target row numbers for each line of text, including headers, spacers, and content lines.
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
 * @brief Data for a single credit line. including the text to display, the column position, and the target row
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
static int16_t current_row[CREDITS_COUNT] = {0}; // Tracks the current row position for each line at any given moment

/* Private Function Prototypes -----------------------------------------------*/
static void EraseCreditLine_(uint8_t row);
static void DrawCredits_(uint32_t frame, uint8_t is_sliding_out);

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
	// Check the maximum screen bounds
	if (row < 1 || row > TERMINAL_HEIGHT)
		return;

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
 * @fn static void DrawCredits_(uint32_t frame, uint8_t is_sliding_out)
 * @brief Handles the animation logic for both sliding in and sliding out the credit lines. During the slide-in phase,
 * lines will move upwards from below the terminal into their target positions with a staggered delay. During the
 * slide-out phase, lines will continue moving upwards off the screen except for the footer which locks to the centre.
 * The function calculates dynamic delays based on the distance to the target row to create a smooth animation effect.
 * @param frame The current frame index provided by the scene manager.
 * @param is_sliding_out Flag indicating whether the credits are in the slide-out phase or slide-in phase.
 */
static void DrawCredits_(uint32_t frame, uint8_t is_sliding_out)
{
	// Calculate new positions and clear old artifacts
	for (int i = 0; i < CREDITS_COUNT; i++)
	{
		// Skip updating this line until its staggered start time has been reached
		const uint32_t starting_frame = i * STAGGER_DELAY;
		if (frame < starting_frame)
			continue;

		// Determine the target row for this line based on whether to slide out of the screen completely or not
		int32_t target;
		if (is_sliding_out == TRUE)
		{
			target = (i == (CREDITS_COUNT - 1)) ? TITLE_TARGET : -1;
		} else
		{
			target = (int32_t)credits[i].target_row;
		}

		// Calculate the distance from the current row to the target row for this credit line
		int32_t distance = current_row[i] - target;
		if (distance > 0)
		{
			// Delaying animation by distance to target row
			uint8_t dynamic_delay = (is_sliding_out == TRUE) ? 2 : (2 + (STAGGER_DELAY / (distance + 1)));

			// Move and render if after correct number of frames based on distance to target row
			if ((frame % dynamic_delay) == 0)
			{
				// Only erase if the row is actually on the screen
				if (current_row[i] >= 1 && current_row[i] <= TERMINAL_HEIGHT)
					EraseCreditLine_(current_row[i]);

				current_row[i]--;

				// Only print if the new position is actually on the screen
				if (current_row[i] >= 1 && current_row[i] <= TERMINAL_HEIGHT)
					TerminalPrintString(credits[i].text, credits[i].col, current_row[i]);
			}
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
 * @brief Scene management loop, dividing the scene into distinct phases:
 * 1. Slide-in (staggered entry)
 * 2. Hold (static display)
 * 3. Slide-out (transition to next scene/title lock)
 * @param scene_frame The current frame index provided by the scene manager.
 */
void SceneCreditsRender(uint32_t scene_frame)
{
	// During the slide-in phase, animate lines sliding up from the bottom with staggered delays
	if (scene_frame < SLIDE_IN_END)
	{
		DrawCredits_(scene_frame, FALSE);
		return;
	}

	// Hold phase, keeping the credits static on the screen without changes for reading
	if (scene_frame < HOLD_END)
		return;

	// During the slide-out phase, animate lines sliding up and off the screen
	// except for the footer which locks to the centre
	DrawCredits_(scene_frame, TRUE);
}