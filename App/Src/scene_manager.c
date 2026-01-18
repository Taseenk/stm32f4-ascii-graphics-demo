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

/* Private Variables ---------------------------------------------------------*/
static uint8_t total_scenes = SCENE_MATRIX_FALLING;
SceneID_t current_scene = SCENE_MATRIX_GLITCH;

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
		current_scene = (current_scene + 1) % total_scenes;

		// Reset the terminal display for the new scene
		TerminalClearAndHome();
	}

	// Scene-specific logic
	switch (current_scene) {
	case SCENE_MATRIX_GLITCH:
		time_in_scene = frame_count % interval;
		// Full Brightness
		if (time_in_scene < 20) {
			SerialPrint(FG_BRIGHT_GREEN);

			// Spawn random characters
			MatrixCharacterNoise(frame_count, 10);

			// Occasional light dissolving to keep the screen from getting too crowded
			if (frame_count % 3 == 0)
				MatrixCharacterDissolve(frame_count, 5);
		}
		// Light Dimming
		else if (time_in_scene < 140) {
			SerialPrint(FG_MEDIUM_GREEN);

			// Spawn fewer new characters, dissolve more existing ones
			MatrixCharacterNoise(frame_count, 4);
			MatrixCharacterDissolve(frame_count, 15);
		}
		// Deeper Fade
		else {
			SerialPrint(FG_DARK_GREEN);

			// Very few new characters and keep dissolving
			MatrixCharacterNoise(frame_count, 2);
			MatrixCharacterDissolve(frame_count, 15);
		}

		break;
	case SCENE_MATRIX_FALLING:
		// TODO: Implement standard rain/falling character logic
		time_in_scene = frame_count % interval;
		break;
	default:
		break;
	}
}
