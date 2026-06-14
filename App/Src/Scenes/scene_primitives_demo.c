/**
 ******************************************************************************
 * @file           : scene_primitives_demo.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_primitives_demo.h"
#include "dsp/fast_math_functions.h"
#include "terminal.h"

// STM32 libraries
#include "arm_math.h"
#include "main.h"
#include <stdint.h>

/* Private Defines -----------------------------------------------------------*/
// Screen centre (1-based coordinates)
#define CENTRE_COL (TERMINAL_WIDTH / 2)  // 40
#define CENTRE_ROW (TERMINAL_HEIGHT / 2) // 12

// Frame duration boundaries for each phase
#define PHASE_LINES_START     0
#define PHASE_LINES_END       39

#define PHASE_RECTS_START     40
#define PHASE_RECTS_END       79

#define PHASE_CIRCLES_START   80
#define PHASE_CIRCLES_END     119

#define PHASE_TRIANGLES_START 120
#define PHASE_TRIANGLES_END   159

#define PHASE_HOLD_START      160

// Characters used for each primitive type
#define CHAR_LINE     '+'
#define CHAR_RECT     '#'
#define CHAR_CIRCLE   'O'
#define CHAR_TRIANGLE '*'

// Max shapes per primitive type
#define LINES_COUNT     8
#define RECTS_COUNT     5
#define CIRCLES_COUNT   4
#define TRIANGLES_COUNT 4

/* Private Variables ---------------------------------------------------------*/

/* Private Function Prototypes -----------------------------------------------*/
static void DrawLines_(uint32_t frame);
static void DrawRects_(uint32_t frame);
static void DrawCircles_(uint32_t frame);
static void DrawTriangles_(uint32_t frame);

/* Private Functions ---------------------------------------------------------*/
static void DrawLines_(uint32_t frame)
{
}

static void DrawRects_(uint32_t frame)
{
}

static void DrawCircles_(uint32_t frame)
{
}

static void DrawTriangles_(uint32_t frame)
{
}

/* Public Functions ----------------------------------------------------------*/
void ScenePrimitivesInit(void)
{
	TerminalClearAndHome();
	TerminalResetStyle();
	TerminalBufferClear();
	TerminalInvisibleCursor();
}

void ScenePrimitivesRender(uint32_t scene_frame)
{
	// Clear the framebuffer before redrawing the full composition this frame
	TerminalBufferClear();

	if (scene_frame >= PHASE_LINES_START)
		DrawLines_(scene_frame);

	if (scene_frame >= PHASE_RECTS_START)
		DrawRects_(scene_frame);

	if (scene_frame >= PHASE_CIRCLES_START)
		DrawCircles_(scene_frame);

	if (scene_frame >= PHASE_TRIANGLES_START)
		DrawTriangles_(scene_frame);

	// Flush the completed framebuffer to the terminal in one DMA transfer
	TerminalBufferFlush();
}