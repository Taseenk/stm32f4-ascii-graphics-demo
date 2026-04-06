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
#define CENTRE_COL     (TERMINAL_WIDTH / 2)
#define CENTRE_ROW     (TERMINAL_HEIGHT / 2)

#define GUTTER_SPACING 2
#define LABEL_COL      (CENTRE_COL - 2)
#define VALUE_COL      (CENTRE_COL + 2)

/* Private Variables ---------------------------------------------------------*/
typedef struct {
	const char *text;
	uint8_t col;
	uint8_t target_row;
	TerminalAttr_t attr;
} CreditLine_t;

static const CreditLine_t credits[] = {
    // Header: Cast
    {"CAST", 38, 3, TERM_ATTR_RESET},

    {"RENDERING", 29, 5, TERM_ATTR_DIM},
    {"ASCII FRAMEBUFFER", 42, 5, TERM_ATTR_RESET},

    {"TRANSPORT", 29, 6, TERM_ATTR_DIM},
    {"UART @ 921600 BAUD", 42, 6, TERM_ATTR_RESET},

    {"TIMING", 32, 7, TERM_ATTR_DIM},
    {"ARM SYSTICK", 42, 7, TERM_ATTR_RESET},

    {"RANDOMNESS", 28, 8, TERM_ATTR_DIM},
    {"HARDWARE TRNG", 42, 8, TERM_ATTR_RESET},

    {"PALETTE", 31, 9, TERM_ATTR_DIM},
    {"XTERM-256 COLOUR", 42, 9, TERM_ATTR_RESET},

    // Header: CREW
    {"CREW", 38, 12, TERM_ATTR_RESET},

    {"HARDWARE", 30, 14, TERM_ATTR_DIM},
    {"STM32F407VG", 42, 14, TERM_ATTR_RESET},

    {"CORE", 34, 15, TERM_ATTR_DIM},
    {"ARM CORTEX-M4", 42, 15, TERM_ATTR_RESET},

    {"CLOCK", 33, 16, TERM_ATTR_DIM},
    {"168 MHZ PLL", 42, 16, TERM_ATTR_RESET},

    {"MEMORY", 32, 17, TERM_ATTR_DIM},
    {"128KB SRAM", 42, 17, TERM_ATTR_RESET},

    // Footer
    {"(C) 2026 ASCII GRAPHICS DEMO", 27, 21, TERM_ATTR_RESET},
};
const uint8_t credits_count = sizeof(credits) / sizeof(credits[0]);

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
	for (int i = 0; i < credits_count; i++)
	{
		// Render each line
		TerminalSetAttribute(credits[i].attr);
		TerminalPrintString(credits[i].text, credits[i].col, credits[i].target_row);
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