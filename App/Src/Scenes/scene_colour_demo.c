/**
 ******************************************************************************
 * @file           : scene_colour_demo.c
 * @brief          : Terminal visual demo showcasing ANSI colour depth. Focusing on terminal
 * colour reproduction. Includes SMPTE calibration patterns, cycling background palettes, and dynamic
 * radial gradients rendered via character-shading LUTs.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_colour_demo.h"
#include "serial_hw.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include "arm_math.h"

/* Private Defines -----------------------------------------------------------*/
// Timing parameters for the colour sequence phases
#define SMPTE_MUTED_DURATION      100
#define BACKGROUND_CYCLE_DURATION (SMPTE_MUTED_DURATION + (2 * (BACKGROUND_COLOUR_COUNT * BACKGROUND_CYCLE_SPEED)))
#define SMPTE_DURATION            (BACKGROUND_CYCLE_DURATION + 100)

// Background colour cycling parameters
#define BACKGROUND_COLOUR_COUNT                                                                                        \
	(BG_DEFAULT - BG_BLACK)      // Total number of standard background colours available to cycle through
#define BACKGROUND_CYCLE_SPEED 9 // Number of frames each background colour is held before advancing to the next

// SMPTE colour bar layout parameters
#define SMPTE_BAR_WIDTH         11 // Standard width (in characters) for a vertical bar
#define SMPTE_WIDE_BAR_WIDTH    12 // Increased with for specific bars to fill terminal width
#define SMPTE_BAR_COUNT         7  // Total amount of vertical bars

#define SMPTE_BAR_HEIGHT        18                            // Total height for the primary colour bars
#define SMPTE_COMPLEMENT_HEIGHT (SMPTE_BAR_HEIGHT + 1)        // Starting row for the complement strip
#define SMPTE_PLUGE_HEIGHT      (SMPTE_COMPLEMENT_HEIGHT + 1) // Starting row for the PLUGE strip

#define SMPTE_LEFT_INDEX        0 // Index of the first bar that is wide (12 chars) in the SMPTE pattern
#define SMPTE_MIDDLE_INDEX      3 // Index of the middle bar that is wide (12 chars) in the SMPTE pattern
#define SMPTE_RIGHT_INDEX       6 // Index of the right bar that is wide (12 chars) in the SMPTE pattern

// Shade LUT parameters for character-based gradients
#define CHADE_LUT_TOTAL_SIZE (sizeof(shade_lut) - 1)
#define CHADE_LUT_MAX_INDEX  (CHADE_LUT_TOTAL_SIZE - 1)

/* Private Variables ---------------------------------------------------------*/
static const char shade_lut[] = "&$XXXXxxxxxx====++++;;;;::::....";
static uint8_t distance_lut[TERMINAL_WIDTH * TERMINAL_HEIGHT];
static uint8_t lut_initialized = FALSE;

/* Private Function Prototypes -----------------------------------------------*/
// Helper functions
static void __DrawSmpteStrip(const BackgroundColour_t *colours, uint8_t count, uint8_t start_row, uint8_t end_row);
static char __GetShadeChar(uint8_t radius);

// Rendering functions
static void __CycleBackgroundColour(uint32_t frame, uint8_t speed);
static void __DrawSmpteMuted(void);
static void __DrawSmpteStandard(void);
static void __RenderRadialPattern(
    uint32_t frame, uint16_t pos_x, uint16_t pos_y, uint8_t is_greyscale, uint8_t is_bg, uint8_t use_lut);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void __DrawSmpteStrip(const BackgroundColour_t *colours, uint8_t count, uint8_t start_row, uint8_t
 * end_row)
 * @brief Draws a vertical strip of colour bars across the terminal based on the provided colour array and dimensions,
 * used to render the SMPTE test pattern bars.
 * @param colours Pointer to the array of background colours for each bar.
 * @param count Number of bars to draw.
 * @param start_row Starting row for the strip.
 * @param end_row Ending row for the strip.
 */
static void __DrawSmpteStrip(const BackgroundColour_t *colours, uint8_t count, uint8_t start_row, uint8_t end_row)
{
	// Character blocks for filling the bars
	static const char bar_normal[] = "           "; // 11 chars
	static const char bar_wide[] = "            ";  // 12 chars

	// Starting column for the first bar
	uint16_t col = 1;

	for (uint8_t i = 0; i < count; i++)
	{
		TerminalSetColour(FG_DEFAULT, colours[i]);

		// Specific indices (0, 3, 6) are wider to align with terminal widths
		uint8_t is_wide = (i == SMPTE_LEFT_INDEX || i == SMPTE_MIDDLE_INDEX || i == SMPTE_RIGHT_INDEX);
		const char *row_block = is_wide ? bar_wide : bar_normal;
		uint16_t bar_width = is_wide ? SMPTE_WIDE_BAR_WIDTH : SMPTE_BAR_WIDTH;

		// Fill the vertical strip for the current bar across the specified row range
		for (uint8_t row = start_row; row <= end_row; row++)
		{
			TerminalSerialPrintString(row_block, col, row);
		}

		// Move the column position for the next bar based on the width of the current bar
		col += bar_width;
	}
}

/**
 * @fn static char __GetShadeChar(uint8_t radius)
 * @brief Retrieves a character from the shade lookup table based on the provided radius value.
 * The radius is used to determine the appropriate shade character for rendering gradients or patterns.
 * @param radius The distance from a central point, used to index into the shade LUT.
 * @return The corresponding shade character from the LUT, or the darkest shade if the radius exceeds the LUT size.
 */
static char __GetShadeChar(uint8_t radius)
{
	// The LUT has 32 characters (0-31 index)
	uint8_t index = radius;

	if (index >= CHADE_LUT_TOTAL_SIZE)
	{
		index = CHADE_LUT_MAX_INDEX;
	}

	return shade_lut[index];
}

/**
 * @fn static void __CycleBackgroundColour(uint32_t frame, uint8_t speed)
 * @brief Cycles the terminal background colour based on the current frame,
 * advancing one colour every `speed` frames and wrapping back to the first
 * colour on completion of a full cycle.
 * @param frame The current scene frame count used to derive the active colour.
 * @param speed The number of frames each colour is held before advancing.
 */
static void __CycleBackgroundColour(uint32_t frame, uint8_t speed)
{
	uint8_t colour_index = ((frame / speed) % BACKGROUND_COLOUR_COUNT) + BG_BLACK;

	TerminalSetColour(FG_DEFAULT, colour_index);
	TerminalClearScreen();
}

/**
 * @fn static void __DrawSmpteMuted(void)
 * @brief Renders a "malfunctioning" or "uncalibrated" SMPTE test pattern.
 * This function mimics this by replacing the standard high-saturation SMPTE
 * colours with grayscale and low-luminance equivalents (Grays, Blacks, and Near-Blacks).
 * It is used as the initial state in to simulate a system error.
 */
static void __DrawSmpteMuted(void)
{
	// These colours represent the uncalibrated version
	// Using mostly grays and blacks to simulate a low-luminance
	static const BackgroundColour_t muted_main_colours[] = {BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY, BG_BLACK,
	                                                        BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY};
	static const BackgroundColour_t muted_complement_colours[] = {BG_BLACK, BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY,
	                                                              BG_BLACK, BG_DARK_GRAY, BG_BLACK};
	static const BackgroundColour_t muted_pluge_colours[] = {BG_BLACK, BG_NEAR_BLACK_1, BG_DARK_GRAY, BG_BLACK,
	                                                         BG_BLACK, BG_NEAR_BLACK_2, BG_BLACK};

	// Draw the top SMPTE bars using the defined colours and dimensions
	__DrawSmpteStrip(muted_main_colours, SMPTE_BAR_COUNT, TERMINAL_STARTING_POS, SMPTE_BAR_HEIGHT);
	__DrawSmpteStrip(muted_complement_colours, SMPTE_BAR_COUNT, SMPTE_COMPLEMENT_HEIGHT, SMPTE_COMPLEMENT_HEIGHT);
	__DrawSmpteStrip(muted_pluge_colours, SMPTE_BAR_COUNT, SMPTE_PLUGE_HEIGHT, TERMINAL_HEIGHT);

	// Reset terminal colours to default after rendering the test pattern
	TerminalResetStyle();
}

/**
 * @fn static void __DrawSmpteStandard(void)
 * @brief Renders a static SMPTE-style colour bar test card filling the entire
 * terminal. The top 18 rows display 7 equal vertical bars of the standard
 * SMPTE colours (white, yellow, cyan, green, magenta, red, blue). Row 19
 * displays the PLUGE/reverse blue strip (blue, black, magenta, black, cyan,
 * black, gray). Rows 20-24 display the PLUGE luminance strip (dark blue,
 * white, purple, black, near-black, dark-gray, black).
 */
static void __DrawSmpteStandard(void)
{
	// SMPTE colour standard colours for each strip
	static const BackgroundColour_t smpte_main_colours[] = {BG_WHITE,   BG_YELLOW, BG_CYAN, BG_GREEN,
	                                                        BG_MAGENTA, BG_RED,    BG_BLUE};
	static const BackgroundColour_t complement_colours[] = {BG_BLUE, BG_BLACK, BG_MAGENTA, BG_BLACK,
	                                                        BG_CYAN, BG_BLACK, BG_WHITE};
	static const BackgroundColour_t pluge_colours[] = {BG_DARK_BLUE, BG_WHITE,        BG_DARK_PURPLE, BG_NEAR_BLACK_1,
	                                                   BG_BLACK,     BG_NEAR_BLACK_2, BG_DARK_GRAY};

	// Draw the top SMPTE bars using the defined colours and dimensions
	__DrawSmpteStrip(smpte_main_colours, SMPTE_BAR_COUNT, TERMINAL_STARTING_POS, SMPTE_BAR_HEIGHT);
	__DrawSmpteStrip(complement_colours, SMPTE_BAR_COUNT, SMPTE_COMPLEMENT_HEIGHT, SMPTE_COMPLEMENT_HEIGHT);
	__DrawSmpteStrip(pluge_colours, SMPTE_BAR_COUNT, SMPTE_PLUGE_HEIGHT, TERMINAL_HEIGHT);

	// Reset terminal colours to default after rendering the test pattern
	TerminalResetStyle();
}

/**
 * @fn static void __RenderRadialPattern(uint32_t frame, uint16_t pos_x, uint16_t pos_y, uint8_t is_greyscale, uint8_t
 * is_bg, uint8_t use_lut)
 * @brief Renders a dynamic radial gradient to the terminal by mapping colors or characters to their distance from a
 * central origin. It supports both greyscale and color modes, with output rendered as solid blocks or character-based
 * shading for either the foreground or background.
 * @param frame The current frame index.
 * @param pos_x The x-coordinate of the center position.
 * @param pos_y The y-coordinate of the center position.
 * @param is_greyscale Flag indicating whether to use greyscale or colour.
 * @param is_bg Flag indicating whether to render as background or foreground.
 * @param use_lut Flag indicating whether to use the lookup table for shading.
 */
static void __RenderRadialPattern(
    uint32_t frame, uint16_t pos_x, uint16_t pos_y, uint8_t is_greyscale, uint8_t is_bg, uint8_t use_lut)
{
	// Move the cursor to the home position before starting the render
	TerminalCursorHome();

	// Go through each position in the terminal
	for (uint16_t y = 1; y <= TERMINAL_HEIGHT; y++)
	{
		for (uint16_t x = 1; x <= TERMINAL_WIDTH; x++)
		{
			// Skip odd rows and columns
			if ((x % 2 != 0) || (y % 2 != 0))
			{
				SerialPrintN(" ", 1);
				continue;
			}

			// Retrieve pre-calculated distance
			uint8_t distance = distance_lut[y * TERMINAL_WIDTH + x];

			// Determine the colour value based on the distance and frame count to create a dynamic pattern
			// Can be either greyscale or colour depending on the mode, and uses the extended colour range
			uint16_t color_val;
			if (is_greyscale == TRUE)
				color_val = EXTENDED_COLOURS_OFFSET + (232 + (((uint32_t)distance + frame) % 24));
			else
				color_val = EXTENDED_COLOURS_OFFSET + (((uint32_t)distance + frame) % 216);

			// Set the appropriate layer based on whether we're rendering a background or foreground pattern
			if (is_bg == TRUE)
				TerminalSetBackgroundColour((BackgroundColour_t)color_val);
			else
				TerminalSetTextColour((ForegroundColour_t)color_val);

			// If using the LUT, print the corresponding shade character based on the distance
			//  otherwise, print a space for a block colour effect
			if (use_lut == TRUE)
			{
				char shade = __GetShadeChar((uint8_t)distance);
				SerialPrintN(&shade, 1);
			} else
			{
				SerialPrintN(" ", 1);
			}
		}
	}
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void ColourDemoInit(void)
 * @brief Initializes the Colour Demo scene by resetting any previous terminal
 * styles to ensure a clean slate for the upcoming colour demonstrations.
 */
void ColourDemoInit(void)
{
	// Reset all previous styles
	TerminalResetStyle();

	// Only calculate the LUT once
	if (lut_initialized == FALSE)
	{
		uint16_t pos_x = TERMINAL_WIDTH / 2;
		uint16_t pos_y = TERMINAL_HEIGHT / 2;

		for (uint16_t y = 1; y <= TERMINAL_HEIGHT; y++)
		{
			// Vertical aspect ratio correction (1.5f)
			float32_t dy = ((float32_t)y - (float32_t)pos_y) * 1.5f;
			float32_t dy_sq = dy * dy;

			for (uint16_t x = 1; x <= TERMINAL_WIDTH; x++)
			{
				float32_t dx = (float32_t)(x - pos_x);
				float32_t sum_sq = (dx * dx) + dy_sq;
				float32_t distance;

				arm_sqrt_f32(sum_sq, &distance);

				// Store the distance in the 1D array
				distance_lut[y * TERMINAL_WIDTH + x] = (uint8_t)distance;
			}
		}
		lut_initialized = TRUE;
	}
}

/**
 * @fn void SmpteCalibrationRender(uint32_t scene_frame)
 * @brief Orchestrates the time-based SMPTE colour calibration demo render.
 * Uses the `scene_frame` counter to transition between three distinct phases:
 * `muted SMPTE bars` (simulating an uncalibrated display), `background colour cycling`,
 * and `standard SMPTE bars` (showcasing the full colour capabilities). Each phase is
 * triggered based on defined frame thresholds, allowing for a dynamic demonstration
 * of terminal colour rendering over time.
 * @param scene_frame The current frame index provided by the scene manager.
 */
void SmpteCalibrationRender(uint32_t scene_frame)
{
	// Early exit if we've exceeded the total duration of the demo to prevent unnecessary rendering
	if (scene_frame >= SMPTE_DURATION)
		return;

	// Phase 1: Display muted SMPTE bars for the initial duration to simulate an uncalibrated display
	if (scene_frame < SMPTE_MUTED_DURATION)
	{
		__DrawSmpteMuted();
		return;
	}

	// Phase 2: Cycle through background colours as calibration process
	if (scene_frame < BACKGROUND_CYCLE_DURATION)
	{
		__CycleBackgroundColour(scene_frame, BACKGROUND_CYCLE_SPEED);
		return;
	}

	// Phase 3: Display the standard SMPTE bars for the remainder of the demo to showcase full colour reproduction
	__DrawSmpteStandard();
}

void RadialGreyscaleRender(uint32_t scene_frame)
{
	__RenderRadialPattern(scene_frame, TERMINAL_WIDTH / 2, TERMINAL_HEIGHT / 2, TRUE, FALSE, TRUE);
}

void RadialColourRender(uint32_t scene_frame)
{
	__RenderRadialPattern(scene_frame, TERMINAL_WIDTH / 2, TERMINAL_HEIGHT / 2, FALSE, TRUE, FALSE);
}