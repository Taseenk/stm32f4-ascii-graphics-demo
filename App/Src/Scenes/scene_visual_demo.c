/**
 ******************************************************************************
 * @file           : scene_visual_demo.c
 * @brief          : Implements the Visual Demo scene, a terminal capabilities
 * showcase that cycles through colour palettes, text styles, and graphics
 * primitives in an animated sequence to demonstrate the rendering power of
 * the STM32F4 ASCII graphics demo.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_visual_demo.h"
#include "terminal.h"

// STM32 libraries
#include "main.h"

/* Private Defines -----------------------------------------------------------*/
// Timing and duration parameters for the different phases of the visual demo scene
#define SMPTE_MUTED_DURATION        100
#define BACKGROUND_CYCLE_DURATION   (SMPTE_MUTED_DURATION + (2 * (BACKGROUND_COLOUR_COUNT * BACKGROUND_CYCLE_SPEED)) )    // Total frames for two full cycles through all background colours
#define SMPTE_DURATION              (BACKGROUND_CYCLE_DURATION + 50)

// Background colour cycling parameters
#define BACKGROUND_COLOUR_COUNT     (BG_DEFAULT - BG_BLACK)     // Total number of standard background colours available to cycle through
#define BACKGROUND_CYCLE_SPEED      9                           // Number of frames each background colour is held before advancing to the next

// SMPTE colour bar parameters
#define SMPTE_BAR_WIDTH             11      // Standard width (in characters) for a vertical bar
#define SMPTE_WIDE_BAR_WIDTH        12      // Increased with for specific bars to fill terminal width
#define SMPTE_BAR_COUNT             7       // Total amount of vertical bars

#define SMPTE_BAR_HEIGHT            18                              // Total height for the primary colour bars
#define SMPTE_COMPLEMENT_HEIGHT     (SMPTE_BAR_HEIGHT + 1)          // Starting row for the complement strip
#define SMPTE_PLUGE_HEIGHT          (SMPTE_COMPLEMENT_HEIGHT + 1)   // Starting row for the PLUGE strip

#define SMPTE_LEFT_INDEX            0   // Index of the first bar that is wide (12 chars) in the SMPTE pattern
#define SMPTE_MIDDLE_INDEX          3   // Index of the middle bar that is wide (12 chars) in the SMPTE pattern
#define SMPTE_RIGHT_INDEX           6   // Index of the right bar that is wide (12 chars) in the SMPTE pattern

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/
static void __CycleBackgroundColour(uint32_t frame, uint8_t speed);
static void __ColourBarsStaticMuted(void);
static void __ColourBarsStatic(void);

/* Private Functions ---------------------------------------------------------*/
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
 * @fn static void __DrawSmpteStrip(const BackgroundColour_t *colours, uint8_t count, uint8_t start_row, uint8_t end_row)
 * @brief Draws a vertical strip of colour bars across the terminal based on the provided colour array and dimensions, used to render the SMPTE test pattern bars.
 * @param colours Pointer to the array of background colours for each bar.
 * @param count Number of bars to draw.
 * @param start_row Starting row for the strip.
 * @param end_row Ending row for the strip.
 */
static void __DrawSmpteStrip(const BackgroundColour_t *colours, uint8_t count, uint8_t start_row, uint8_t end_row)
{
    // Character blocks for filling the bars
    static const char bar_normal[] = "           ";     // 11 chars
    static const char bar_wide[] = "            ";      // 12 chars

    // Starting column for the first bar
    uint16_t col = 1;

    for (uint8_t i = 0; i < count; i++) {
        TerminalSetColour(FG_DEFAULT, colours[i]);

        // Specific indices (0, 3, 6) are wider to align with terminal widths
        uint8_t is_wide = (i == SMPTE_LEFT_INDEX || i == SMPTE_MIDDLE_INDEX || i == SMPTE_RIGHT_INDEX);
        const char *row_block = is_wide ? bar_wide : bar_normal;
        uint16_t bar_width = is_wide ? SMPTE_WIDE_BAR_WIDTH : SMPTE_BAR_WIDTH;

        // Fill the vertical strip for the current bar across the specified row range
        for (uint8_t row = start_row; row <= end_row; row++) {
            TerminalSerialPrintString(row_block, col, row);
        }

        // Move the column position for the next bar based on the width of the current bar
        col += bar_width;
    }
}

/**
 * @fn static void __ColourBarsStaticMuted(void)
 * @brief Renders a "malfunctioning" or "uncalibrated" SMPTE test pattern.
 * This function mimics this by replacing the standard high-saturation SMPTE 
 * colors with grayscale and low-luminance equivalents (Grays, Blacks, and Near-Blacks). 
 * It is used as the initial state in the visual demo to simulate a system error 
 * or boot-up sequence before calibration occurs.
 */
static void __ColourBarsStaticMuted(void)
{
    // These colors represent the uncalibrated version
    // Using mostly grays and blacks to simulate a low-luminance
    static const BackgroundColour_t muted_main_colours[] = {
        BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY
    };
    static const BackgroundColour_t muted_complement_colours[] = {
        BG_BLACK, BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY, BG_BLACK, BG_DARK_GRAY, BG_BLACK
    };
    static const BackgroundColour_t muted_pluge_colours[] = {
        BG_BLACK, BG_NEAR_BLACK_1, BG_DARK_GRAY, BG_BLACK, BG_BLACK, BG_NEAR_BLACK_2, BG_BLACK
    };

    // Draw the top SMPTE bars using the defined colours and dimensions
    __DrawSmpteStrip(muted_main_colours, SMPTE_BAR_COUNT, TERMINAL_STARTING_POS, SMPTE_BAR_HEIGHT);
    __DrawSmpteStrip(muted_complement_colours, SMPTE_BAR_COUNT, SMPTE_COMPLEMENT_HEIGHT, SMPTE_COMPLEMENT_HEIGHT);
    __DrawSmpteStrip(muted_pluge_colours, SMPTE_BAR_COUNT, SMPTE_PLUGE_HEIGHT, TERMINAL_HEIGHT);

    // Reset terminal colours to default after rendering the test pattern
    TerminalResetStyle();
}

/**
 * @fn static void __ColourBarsStatic(void)
 * @brief Renders a static SMPTE-style colour bar test card filling the entire
 * terminal. The top 18 rows display 7 equal vertical bars of the standard
 * SMPTE colours (white, yellow, cyan, green, magenta, red, blue). Row 19
 * displays the PLUGE/reverse blue strip (blue, black, magenta, black, cyan,
 * black, gray). Rows 20-24 display the PLUGE luminance strip (dark blue,
 * white, purple, black, near-black, dark-gray, black).
 */
static void __ColourBarsStatic(void)
{
    // SMPTE colour standard colours for each strip
    static const BackgroundColour_t smpte_main_colours[] = {
        BG_WHITE, BG_YELLOW, BG_CYAN, BG_GREEN, BG_MAGENTA, BG_RED, BG_BLUE
    };
    static const BackgroundColour_t complement_colours[] = {
        BG_BLUE, BG_BLACK, BG_MAGENTA, BG_BLACK, BG_CYAN, BG_BLACK, BG_WHITE
    };
    static const BackgroundColour_t pluge_colours[] = {
        BG_DARK_BLUE, BG_WHITE, BG_DARK_PURPLE, BG_NEAR_BLACK_1, BG_BLACK, BG_NEAR_BLACK_2, BG_DARK_GRAY
    };

    // Draw the top SMPTE bars using the defined colours and dimensions
    __DrawSmpteStrip(smpte_main_colours, SMPTE_BAR_COUNT, TERMINAL_STARTING_POS, SMPTE_BAR_HEIGHT);
    __DrawSmpteStrip(complement_colours, SMPTE_BAR_COUNT, SMPTE_COMPLEMENT_HEIGHT, SMPTE_COMPLEMENT_HEIGHT);
    __DrawSmpteStrip(pluge_colours, SMPTE_BAR_COUNT, SMPTE_PLUGE_HEIGHT, TERMINAL_HEIGHT);

    // Reset terminal colours to default after rendering the test pattern
    TerminalResetStyle();
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void VisualDemoRender(uint32_t scene_frame)
 * @brief
 * @param scene_frame The current frame index provided by the scene manager.
 */
void VisualDemoRender(uint32_t scene_frame)
{
    if (scene_frame < SMPTE_MUTED_DURATION) 
    {
        __ColourBarsStaticMuted();
    } 
    else if (scene_frame < BACKGROUND_CYCLE_DURATION) 
    {
        __CycleBackgroundColour(scene_frame, BACKGROUND_CYCLE_SPEED);
    } 
    else if (scene_frame < SMPTE_DURATION)  
    {
        __ColourBarsStatic();
    }
    else
    {
        return;
    }
}