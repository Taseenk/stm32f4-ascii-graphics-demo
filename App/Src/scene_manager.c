/**
 ******************************************************************************
 * @file           : scene_manager.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_manager.h"
#include "gfx_matrix_stream.h"
#include "serial_hw.h"
#include "terminal.h"

// Standard libraries
#include <stdio.h>

/* Private Variables ---------------------------------------------------------*/
SceneID_t current_scene = SCENE_MATRIX_FALLING;

/* Private Function Prototypes -----------------------------------------------*/

/* Private Functions ---------------------------------------------------------*/

/* Public Functions ----------------------------------------------------------*/
void SceneManager(uint32_t frame_count)
{
	uint16_t interval = 300; // Duration of each scene before switching
	uint16_t time_in_scene;  // Time spent in the current scene

	// Scene transition at the end of every interval
	if (frame_count % interval == 0) {
		// Move to next scene index, wrapping back to 0 at the end of the list
		current_scene = (SceneID_t)((current_scene + 1) % SCENE_TOTAL_COUNT);

		// Reset the terminal display for the new scene
		TerminalClearAndHome();
	}

	// Scene-specific logic
	switch (current_scene) {
	case SCENE_MATRIX_GLITCH:
		// Determine time spent in the current scene
		time_in_scene = frame_count % interval;

		// Adjust text colour based on time in scene
		if (time_in_scene == 0) {
			TerminalSetTextColour(FG_BRIGHT_GREEN);
		} else if (time_in_scene == 21) {
			TerminalSetTextColour(FG_MEDIUM_GREEN);
		} else if (time_in_scene == 141) {
			TerminalSetTextColour(FG_DARK_GREEN);
		}

		// Full Brightness
		if (time_in_scene < 20) {
			// Spawn random characters
			MatrixCharacterNoise(frame_count, 20);

			// Occasional light dissolving to keep the screen from getting too crowded
			if (frame_count % 3 == 0)
				MatrixCharacterDissolve(frame_count, 5);
		}
		// Light Dimming
		else if (time_in_scene < 140) {
			// Spawn fewer new characters, dissolve more existing ones
			MatrixCharacterNoise(frame_count, 4);
			MatrixCharacterDissolve(frame_count, 15);
		}
		// Deeper Fade
		else {
			// Very few new characters and keep dissolving
			MatrixCharacterNoise(frame_count, 2);
			MatrixCharacterDissolve(frame_count, 15);
		}

		break;
	case SCENE_MATRIX_FALLING:
		// TODO: Implement standard rain/falling character logic
		time_in_scene = frame_count % interval;
		break;
	case SCENE_MATRIX_FALLING_GLITCH:
		// TODO: Implement combined rain/falling and glitch character logic
		time_in_scene = frame_count % interval;
		break;
	default:
		break;
	}
}

void FPSDisplay(uint32_t fps_counter)
{
	// Current calculated FPS value
	uint32_t fps = fps_counter;

	// Buffer for UART/Terminal debug strings
	char debug_msg[64];

	// Output FPS info to the top-left of the terminal
	TerminalSetTextColour(FG_BLACK);
	TerminalSetCursorPos(1, 1);
	sprintf(debug_msg, "FPS: %-3lu", (unsigned long)fps);
	SerialPrint(debug_msg);

	// Restore the default terminal styling
	TerminalSetColour(FG_DEFAULT, BG_DEFAULT);
}
