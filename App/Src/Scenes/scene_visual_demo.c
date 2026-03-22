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
#define BACKGROUND_COLOUR_COUNT     (BG_DEFAULT - BG_BLACK)     // Total number of standard background colours available to cycle through
#define BACKGROUND_CYCLE_SPEED      9                           // Number of frames each background colour is held before advancing to the next
#define BACKGROUND_CYCLE_DURATION   (2 * (BACKGROUND_COLOUR_COUNT * BACKGROUND_CYCLE_SPEED))    // Total frames for two full cycles through all background colours

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/
static void __CycleBackgroundColour(uint32_t frame, uint8_t speed);

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

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void VisualDemoRender(uint32_t scene_frame)
 * @brief
 * @param scene_frame The current frame index provided by the scene manager.
 */
void VisualDemoRender(uint32_t scene_frame)
{
    if (scene_frame < BACKGROUND_CYCLE_DURATION) {
        __CycleBackgroundColour(scene_frame, BACKGROUND_CYCLE_SPEED);
        return;
    }
}