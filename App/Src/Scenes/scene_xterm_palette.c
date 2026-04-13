/**
 ******************************************************************************
 * @file           : scene_xterm_palette.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "scene_xterm_palette.h"
#include "terminal.h"

// Standard libraries
#include <stdio.h>

/* Private Defines -----------------------------------------------------------*/
// Grayscale Ramp Layout
#define GRAY_START_ROW 22 // Starting row for the grayscale section
#define GRAY_ROW_COUNT 2  // Number of rows used to display the grayscale ramp
#define GRAY_TEXT_COL  1  // Column position for the "Grays:" label
#define GRAY_START_COL 9  // Starting X-coordinate for the first colour cell
#define GRAY_PER_ROW   12 // Number of grayscale blocks to display per row

// Xterm Colour Mapping
#define GRAY_BASE_COLOUR 232 // The starting index for grayscale in the xterm-256 palette
#define CELL_WIDTH       5   // Horizontal character spacing for each gray cell

// Buffer and Formatting
#define TEXT_COLOUR_SIZE 8 // Buffer size for formatting gray colour strings

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/
static void DrawGrayScaleRamp(void);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void DrawGrayScaleRamp(void)
 * @brief Renders a grayscale ramp at the bottom of the screen using the extended xterm-256 colour palette,
 * demonstrating the range of available shades from near-black to white. Each shade is labeled with its corresponding
 * colour code for reference.
 */
static void DrawGrayScaleRamp(void)
{
	// Print the section label for the grayscale ramp
	TerminalPrintString("Grays:", GRAY_TEXT_COL, GRAY_START_ROW);

	// Split the 24 grayscale shades into two rows of 12
	for (uint8_t row_idx = 0; row_idx < GRAY_ROW_COUNT; row_idx++)
	{
		// Calculate the row position and base colour code for this row
		uint16_t row = GRAY_START_ROW + row_idx;
		uint16_t base_colour = GRAY_BASE_COLOUR + (GRAY_PER_ROW * row_idx);
		ForegroundColour_t fg_colour = (row_idx == 1) ? FG_BLACK : FG_DEFAULT;

		// Iterare through the shades for this row
		for (uint16_t scale_ramp = 0; scale_ramp < GRAY_PER_ROW; scale_ramp++)
		{
			// Calculate the column position and actual colour code for this shade
			uint16_t col = GRAY_START_COL + (scale_ramp * CELL_WIDTH);
			uint16_t colour = base_colour + scale_ramp + EXTENDED_COLOURS_OFFSET;

			// Format the colour code label to display within the cell
			char text_colour[TEXT_COLOUR_SIZE];
			snprintf(text_colour, sizeof(text_colour), " %3u ", (colour - EXTENDED_COLOURS_OFFSET));

			// Set the foreground colour and print the colour code label at the calculated position
			TerminalSetColour(fg_colour, colour);
			TerminalPrintString(text_colour, col, row);
		}
	}
}

/* Public Functions ----------------------------------------------------------*/
void SceneXtermPaletteInit(void)
{
	// Clear the screen and reset all attributes to default
	TerminalClearAndHome();
	TerminalResetStyle();

	// Render each section
	DrawGrayScaleRamp();
}

void SceneXtermPaletteRender(uint32_t scene_frame)
{
}