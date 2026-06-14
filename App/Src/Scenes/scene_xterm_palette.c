/**
 ******************************************************************************
 * @file           : scene_xterm_palette.c
 * @brief          : Implements the xterm-256 colour palette test card scene.
 * Renders all 256 xterm colours across two sections: the 216-colour
 * 6x6x6 RGB cube (indices 16-231) arranged in three paired 6x6 bands,
 * and the 24-step grayscale ramp (indices 232-255) in two rows of twelve.
 * Each cell displays its colour index as a label. The scene is fully
 * static — all drawing occurs once in init with no per-frame updates.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "scene_xterm_palette.h"
#include "terminal.h"

// Standard libraries
#include <stdio.h>

/* Private Defines -----------------------------------------------------------*/
#define START_COL        1 // Global starting column
#define CELL_WIDTH       5 // Width of each colour block including text
#define TEXT_COLOUR_SIZE 8 // Buffer size for formatting colour strings

// Colour Cube (6x6x6) Layout
#define COLOUR_START_ROW 1 // Top row for the colour cube
#define COLOUR_HEIGHT    6 // Dimensions of the 6x6x6 cube segments

#define COLOUR_2ND_COL                                                                                                 \
	(START_COL + ((COLOUR_HEIGHT) * CELL_WIDTH) + COLOUR_HORIZONTAL_GAP) // Offset for right-hand cube blocks

#define COLOUR_VERTICAL_GAP   1  // Rows between stacked cube segments
#define COLOUR_HORIZONTAL_GAP 2  // Columns between stacked cube segments
#define RED_STEP              36 // Multiplier for red component
#define GREEN_STEP            6  // Multiplier for green component
#define COLOUR_BASE_COLOUR    16 // Start of the 6x6x6 cube colour

// Grayscale Ramp Layout
#define GRAY_START_ROW   22  // Starting row for the grayscale section
#define GRAY_ROW_COUNT   2   // Number of rows used to display the grayscale ramp
#define GRAY_START_COL   9   // Starting X-coordinate for the first colour cell
#define GRAY_PER_ROW     12  // Number of grayscale blocks to display per row
#define GRAY_BASE_COLOUR 232 // The starting index for grayscale in the xterm-256 palette

/* Private Function Prototypes -----------------------------------------------*/
static void DrawColouredCell_(uint16_t colour, uint16_t col, uint16_t row, ForegroundColour_t fg);

static void DrawColourCube_(void);
static void DrawGrayScaleRamp_(void);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static void DrawColouredCell_(uint16_t colour, uint16_t col, uint16_t row, ForegroundColour_t fg_colour)
 * @brief Renders a single cell in the colour cube or grayscale ramp with the specified background colour and
 * foreground colour for the text label.
 * @param colour The xterm-256 colour code to use for the background of the cell
 * @param col The column position to start printing the cell (1-based index)
 * @param row The row position to print the cell (1-based index)
 * @param fg_colour The foreground colour to use for the text label within the cell
 */
static void DrawColouredCell_(uint16_t colour, uint16_t col, uint16_t row, ForegroundColour_t fg_colour)
{
	// Format the colour code label to display within the cell
	char text_buffer[TEXT_COLOUR_SIZE];

	// Convert the colour code to a string
	int len = snprintf(text_buffer, sizeof(text_buffer), " %3u ", (unsigned int)(colour - EXTENDED_COLOURS_OFFSET));

	// Check if sprintf failed (len < 0) or if the formatted string exceeded the terminal size
	if (len <= 0 || (size_t)len >= sizeof(text_buffer))
		return;

	// Set the foreground colour and print the colour code label at the calculated position
	TerminalSetColour(fg_colour, colour);
	TerminalPrintString(text_buffer, col, row);
}

/**
 * @fn static void DrawColourCube_(void)
 * @brief Renders the 216-color xterm-256 cube (16-231). The function iterates through red, green,
 * and blue components to build the 6x6x6 color cube. It organizes the output into two columns of
 * three 6x6 grids for better display density.
 */
static void DrawColourCube_(void)
{
	// Reset terminal style to ensure default foreground colour is used for the colour cube labels
	TerminalResetStyle();

	// Iterate through green component as the primary loop to group grids
	for (uint16_t green = 0; green < COLOUR_HEIGHT; green++)
	{
		// Arrange grids in pairs where every 2 green levels starts a new vertical band
		uint8_t band_idx = green / 2;
		uint16_t starting_row = COLOUR_START_ROW + ((COLOUR_HEIGHT + COLOUR_VERTICAL_GAP) * band_idx);

		// Toggle column based on green parity to create a two-column grid layout
		uint16_t starting_col = ((green % 2) == 0) ? START_COL : COLOUR_2ND_COL;

		// Adjust contrast based on green intensity for better readability
		ForegroundColour_t fg_colour = ((green < 2) == 0) ? FG_BLACK : FG_DEFAULT;

		// Iterate through red and blue components to fill out the 6x6 grid for this green level
		for (uint16_t red = 0; red < COLOUR_HEIGHT; red++)
		{
			for (uint16_t blue = 0; blue < COLOUR_HEIGHT; blue++)
			{
				// Calculate the col, row and colour for this cell based on the current red, green, and blue indices
				uint16_t col = starting_col + (blue * CELL_WIDTH);
				uint16_t row = starting_row + red;
				uint16_t colour =
				    EXTENDED_COLOURS_OFFSET + COLOUR_BASE_COLOUR + (red * RED_STEP) + (green * GREEN_STEP) + blue;

				// Draw the coloured cell with the appropriate foreground colour
				DrawColouredCell_(colour, col, row, fg_colour);
			}
		}
	}
}

/**
 * @fn static void DrawGrayScaleRamp_(void)
 * @brief Renders a grayscale ramp at the bottom of the screen using the extended xterm-256 colour palette,
 * demonstrating the range of available shades from near-black to white. Each shade is labeled with its corresponding
 * colour code for reference.
 */
static void DrawGrayScaleRamp_(void)
{
	// Reset terminal style to ensure default foreground colour is used for the grayscale labels
	TerminalResetStyle();

	// Print the section label for the grayscale ramp
	TerminalPrintString("Grays:", START_COL, GRAY_START_ROW);

	// Split the 24 grayscale shades into two rows of 12
	for (uint8_t row_idx = 0; row_idx < GRAY_ROW_COUNT; row_idx++)
	{
		// Calculate the row position and base colour code for this row
		uint16_t row = GRAY_START_ROW + row_idx;
		uint16_t base_colour = GRAY_BASE_COLOUR + (GRAY_PER_ROW * row_idx);
		ForegroundColour_t fg_colour = (row_idx == 1) ? FG_BLACK : FG_DEFAULT;

		// Iterate through the shades for this row
		for (uint16_t scale_ramp = 0; scale_ramp < GRAY_PER_ROW; scale_ramp++)
		{
			// Calculate the column position and actual colour code for this shade
			uint16_t col = GRAY_START_COL + (scale_ramp * CELL_WIDTH);
			uint16_t colour = base_colour + scale_ramp + EXTENDED_COLOURS_OFFSET;

			// Draw the coloured cell with the appropriate foreground colour
			DrawColouredCell_(colour, col, row, fg_colour);
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
	DrawColourCube_();
	DrawGrayScaleRamp_();
}

void SceneXtermPaletteRender(uint32_t scene_frame)
{
	(void)scene_frame; // Static scene, no per-frame updates
}