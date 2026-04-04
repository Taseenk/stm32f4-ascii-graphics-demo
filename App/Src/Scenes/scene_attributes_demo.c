/**
 ******************************************************************************
 * @file           : scene_attributes_demo.c
 * @brief          : Implements the attributes Demo scene, a VT100 attribute
 * torture test that fills the 80x24 terminal with a showcase of every
 * supported text attribute — bold, underline, blink, inverse, dim,
 * strikethrough — and their common combinations.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_attributes_demo.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <string.h>

/* Private Defines -----------------------------------------------------------*/
// Constants for dynamic part of the scene
#define ASCII_PATTERN_SPEED   15  // Number of frames before shifting the background pattern
#define ASCII_PRINTABLE_START 32  // The '!' character
#define ASCII_PRINTABLE_END   126 // The '~' character

// UI Text Content: Header & Footer
#define TITLE_TEXT      "STM32F4 ANSI/VT100 Attribute Demonstration"
#define TITLE_TEXT_SIZE (sizeof(TITLE_TEXT) - 1)
#define FOOTER_TEXT     "STM32F407VG @ 168MHz  |  ARM Cortex-M4  |  (C) 2026 Taseen"

// UI Text Content: Single Attribute Samples
#define NORMAL_TEXT        "This is a normal line"
#define BOLD_TEXT          "This is a bold line"
#define UNDERLINE_TEXT     "This is an underlined line"
#define BLINK_TEXT         "This is a blinking line"
#define INVERSE_TEXT       "This is inverse video"

#define DIM_TEXT           "This is dim/faint text"
#define STRIKETHROUGH_TEXT "This is strikethrough text"

// UI Text Content: Multi-Attribute Samples
#define BOLD_UNDERLINE_TEXT  "This is bold + underline"
#define BOLD_BLINK_TEXT      "This is bold + blink"
#define BOLD_INVERSE_TEXT    "This is bold + inverse"
#define UNDERLINE_BLINK_TEXT "This is underline + blink"

// UI Text Content: Grid Labels
#define GRID_SAMPLE_TEXT      "grid sample"
#define GRID_HEADER_NORMAL    "Normal"
#define GRID_HEADER_BOLD      "Bold"
#define GRID_HEADER_UNDERLINE "Underline"
#define GRID_HEADER_BLINK     "Blink"
#define GRID_HEADER_INVERSE   "Inverse"

// UI Layout (Rows & Columns): Rows
#define ROW_TITLE  1
#define ROW_FOOTER 24

// UI Layout (Rows & Columns): Columns
#define GRID_COL_NORMAL    1
#define GRID_COL_BOLD      17
#define GRID_COL_UNDERLINE 33
#define GRID_COL_BLINK     49
#define GRID_COL_INVERSE   65

// UI Layout (Rows & Columns): Single Attributes Section
#define ROW_NORMAL        (ROW_TITLE + 2)
#define ROW_BOLD          (ROW_NORMAL + 1)
#define ROW_UNDERLINE     (ROW_BOLD + 1)
#define ROW_BLINK         (ROW_UNDERLINE + 1)
#define ROW_INVERSE       (ROW_BLINK + 1)
#define ROW_DIM           (ROW_INVERSE + 1)
#define ROW_STRIKETHROUGH (ROW_DIM + 1)

// UI Layout (Rows & Columns): Combinations Section
#define ROW_BOLD_UNDERLINE  (ROW_STRIKETHROUGH + 2)
#define ROW_BOLD_BLINK      (ROW_BOLD_UNDERLINE + 1)
#define ROW_BOLD_INVERSE    (ROW_BOLD_BLINK + 1)
#define ROW_UNDERLINE_BLINK (ROW_BOLD_INVERSE + 1)

// UI Layout (Rows & Columns): Grid Section
#define ROW_GRID_HEADER  (ROW_UNDERLINE_BLINK + 2)
#define ROW_GRID_NORMAL  (ROW_GRID_HEADER + 1)
#define ROW_GRID_BOLD    (ROW_GRID_NORMAL + 1)
#define ROW_GRID_INVERSE (ROW_GRID_BOLD + 1)

// UI Layout (Rows & Columns): Animated Section
#define ROW_ASCII_SET (ROW_GRID_INVERSE + 2)

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/
static void DrawTitleBar_(void);
static void DrawSingleAttributeLines_(void);
static void DrawCombinationGrid_(void);
static void DrawAsciiSet_(uint32_t frame, uint8_t speed);
static void DrawFooter_(void);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void DrawTitleBar_(void)
 * @brief Renders the title bar at row 1 in inverse video to mimic the
 * bordered heading style.
 */
static void DrawTitleBar_(void)
{
	// Initialize the title text bufferm for the full width with spaces and null-terminate it
	char title_buffer[TERMINAL_WIDTH + 1];
	memset(title_buffer, ' ', TERMINAL_WIDTH);
	title_buffer[TERMINAL_WIDTH] = '\0';

	// Center the title text in the title buffer
	uint16_t start_pos = (TERMINAL_WIDTH / 2) - (TITLE_TEXT_SIZE / 2);
	memcpy(&title_buffer[start_pos], TITLE_TEXT, TITLE_TEXT_SIZE);

	// Output the title bar with inverse video styling
	TerminalSetAttribute(TERM_ATTR_REVERSE);
	TerminalPrintString(title_buffer, 1, ROW_TITLE);
	TerminalSetAttribute(TERM_ATTR_RESET);
}

/**
 * @fn static void DrawSingleAttributeLines_(void)
 * @brief Renders one line per ANSI text attribute and common two-attribute
 * combinations, each demonstrating a single capability of the terminal.
 */
static void DrawSingleAttributeLines_(void)
{
	// Normal — no attribute applied, baseline reference
	TerminalPrintString(NORMAL_TEXT, 1, ROW_NORMAL);

	// Bold
	TerminalSetAttribute(TERM_ATTR_BOLD);
	TerminalPrintString(BOLD_TEXT, 1, ROW_BOLD);
	TerminalSetAttribute(TERM_ATTR_RESET_BOLD);

	// Underline
	TerminalSetAttribute(TERM_ATTR_UNDERLINE);
	TerminalPrintString(UNDERLINE_TEXT, 1, ROW_UNDERLINE);
	TerminalSetAttribute(TERM_ATTR_RESET_UNDERLINE);

	// Blink
	TerminalSetAttribute(TERM_ATTR_BLINK);
	TerminalPrintString(BLINK_TEXT, 1, ROW_BLINK);
	TerminalSetAttribute(TERM_ATTR_RESET_BLINK);

	// Inverse
	TerminalSetAttribute(TERM_ATTR_REVERSE);
	TerminalPrintString(INVERSE_TEXT, 1, ROW_INVERSE);
	TerminalSetAttribute(TERM_ATTR_RESET_REVERSE_MODE);

	// Dim
	TerminalSetAttribute(TERM_ATTR_DIM);
	TerminalPrintString(DIM_TEXT, 1, ROW_DIM);
	TerminalSetAttribute(TERM_ATTR_RESET_BOLD);

	// Strikethrough
	TerminalSetAttribute(TERM_ATTR_STRIKE);
	TerminalPrintString(STRIKETHROUGH_TEXT, 1, ROW_STRIKETHROUGH);
	TerminalSetAttribute(TERM_ATTR_RESET_STRIKE);

	// Bold + Underline
	TerminalSetAttribute(TERM_ATTR_BOLD);
	TerminalSetAttribute(TERM_ATTR_UNDERLINE);
	TerminalPrintString(BOLD_UNDERLINE_TEXT, 1, ROW_BOLD_UNDERLINE);
	TerminalSetAttribute(TERM_ATTR_RESET);

	// Bold + Blink
	TerminalSetAttribute(TERM_ATTR_BOLD);
	TerminalSetAttribute(TERM_ATTR_BLINK);
	TerminalPrintString(BOLD_BLINK_TEXT, 1, ROW_BOLD_BLINK);
	TerminalSetAttribute(TERM_ATTR_RESET);

	// Bold + Inverse
	TerminalSetAttribute(TERM_ATTR_BOLD);
	TerminalSetAttribute(TERM_ATTR_REVERSE);
	TerminalPrintString(BOLD_INVERSE_TEXT, 1, ROW_BOLD_INVERSE);
	TerminalSetAttribute(TERM_ATTR_RESET);

	// Underline + Blink
	TerminalSetAttribute(TERM_ATTR_UNDERLINE);
	TerminalSetAttribute(TERM_ATTR_BLINK);
	TerminalPrintString(UNDERLINE_BLINK_TEXT, 1, ROW_UNDERLINE_BLINK);
	TerminalSetAttribute(TERM_ATTR_RESET);
}

/**
 * @fn static void DrawCombinationGrid_(void)
 * @brief Renders a 5x5 grid showcasing the combination of each attribute with
 * every other attribute, allowing for easy visual comparison of how attributes
 * interact when layered on top of each other.
 */
static void DrawCombinationGrid_(void)
{
	// Header row — label each column
	TerminalPrintString(GRID_HEADER_NORMAL, GRID_COL_NORMAL, ROW_GRID_HEADER);
	TerminalPrintString(GRID_HEADER_BOLD, GRID_COL_BOLD, ROW_GRID_HEADER);
	TerminalPrintString(GRID_HEADER_UNDERLINE, GRID_COL_UNDERLINE, ROW_GRID_HEADER);
	TerminalPrintString(GRID_HEADER_BLINK, GRID_COL_BLINK, ROW_GRID_HEADER);
	TerminalPrintString(GRID_HEADER_INVERSE, GRID_COL_INVERSE, ROW_GRID_HEADER);

	// Grid row 1 — base attributes applied individually
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_NORMAL, ROW_GRID_NORMAL);

	TerminalPrint(ANSI_BOLD);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_BOLD, ROW_GRID_NORMAL);
	TerminalPrint(ANSI_RESET_BOLD);

	TerminalPrint(ANSI_UNDERLINE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_UNDERLINE, ROW_GRID_NORMAL);
	TerminalPrint(ANSI_RESET_UNDERLINE);

	TerminalPrint(ANSI_BLINK);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_BLINK, ROW_GRID_NORMAL);
	TerminalPrint(ANSI_RESET_BLINK);

	TerminalPrint(ANSI_REVERSE_MODE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_INVERSE, ROW_GRID_NORMAL);
	TerminalPrint(ANSI_RESET_REVERSE_MODE);

	// Grid row 2 — bold added on top of each base attribute
	TerminalPrint(ANSI_BOLD);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_NORMAL, ROW_GRID_BOLD);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_BOLD);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_BOLD, ROW_GRID_BOLD);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_BOLD);
	TerminalPrint(ANSI_UNDERLINE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_UNDERLINE, ROW_GRID_BOLD);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_BOLD);
	TerminalPrint(ANSI_BLINK);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_BLINK, ROW_GRID_BOLD);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_BOLD);
	TerminalPrint(ANSI_REVERSE_MODE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_INVERSE, ROW_GRID_BOLD);
	TerminalPrint(ANSI_RESET_STYLE);

	// Grid row 3 — inverse added on top of each base attribute
	TerminalPrint(ANSI_REVERSE_MODE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_NORMAL, ROW_GRID_INVERSE);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_BOLD);
	TerminalPrint(ANSI_REVERSE_MODE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_BOLD, ROW_GRID_INVERSE);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_UNDERLINE);
	TerminalPrint(ANSI_REVERSE_MODE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_UNDERLINE, ROW_GRID_INVERSE);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_BLINK);
	TerminalPrint(ANSI_REVERSE_MODE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_BLINK, ROW_GRID_INVERSE);
	TerminalPrint(ANSI_RESET_STYLE);

	TerminalPrint(ANSI_BOLD);
	TerminalPrint(ANSI_UNDERLINE);
	TerminalPrint(ANSI_REVERSE_MODE);
	TerminalPrintString(GRID_SAMPLE_TEXT, GRID_COL_INVERSE, ROW_GRID_INVERSE);
	TerminalPrint(ANSI_RESET_STYLE);
}

/**
 * @fn static void DrawAsciiSet_(uint32_t frame, uint8_t speed)
 * @brief Renders the standard printable ASCII character set (32-126) in a single row at the bottom of the screen, with
 * a dynamic alternating background pattern that shifts every few frames to demonstrate how attributes interact with
 * character output.
 * @param frame The current frame count, used to calculate the animation offset
 * @param speed The number of frames to wait before shifting the background pattern
 */
static void DrawAsciiSet_(uint32_t frame, uint8_t speed)
{
	// Calculate an offset for animating the background pattern based on the frame count and speed
	uint32_t offset = (frame / speed);

	// Move to the start position for this row
	TerminalSetCursorPos(1, ROW_ASCII_SET);

	// Print the standard printable ASCII character set (32-126), streaming characters individually
	char single_char[2] = {0, '\0'};
	for (int ascii_char = ASCII_PRINTABLE_START; ascii_char <= ASCII_PRINTABLE_END; ascii_char++)
	{
		// Toggle background styling based on even/odd index
		if ((ascii_char + offset) % 2 == 0)
			TerminalSetAttribute(TERM_ATTR_REVERSE);
		else
			TerminalSetAttribute(TERM_ATTR_RESET_REVERSE_MODE);

		single_char[0] = (char)ascii_char;
		TerminalPrint(single_char);
	}

	// Reset style after finishing the set
	TerminalSetAttribute(TERM_ATTR_RESET);
}

/**
 * @fn static void DrawFooter_(void)
 * @brief Renders the footer text at the bottom of the screen in dimmed style to
 * provide a subtle, non-distracting information bar.
 */
static void DrawFooter_(void)
{
	TerminalSetAttribute(TERM_ATTR_DIM);
	TerminalPrintString(FOOTER_TEXT, 1, ROW_FOOTER);
	TerminalSetAttribute(TERM_ATTR_RESET_BOLD);
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void SceneAttributesInit(void)
 * @brief Initializes the attributes demo scene by rendering the static layout of
 * text samples showcasing each ANSI attribute and their combinations.
 */
void SceneAttributesInit(void)
{
	// Clear the screen and reset all attributes to default
	TerminalClearAndHome();
	TerminalResetStyle();

	// Render each section of the demo layout
	DrawTitleBar_();
	DrawSingleAttributeLines_();
	DrawCombinationGrid_();
	DrawFooter_();
}

/**
 * @fn void SceneAttributesRender(uint32_t scene_frame)
 * @brief The attributes demo scene is static and does not require any updates
 * during rendering, so this function is intentionally left empty.
 */
void SceneAttributesRender(uint32_t scene_frame)
{
	// Animated ASCII set at the bottom to demonstrate attributes with character output
	DrawAsciiSet_(scene_frame, ASCII_PATTERN_SPEED);
}