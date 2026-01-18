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
#include "terminal.h"

/* Private Variables ---------------------------------------------------------*/
static uint8_t total_scenes = SCENE_MATRIX_FALLING;
SceneID_t current_scene = SCENE_MATRIX_GLITCH;

/* Private Function Prototypes -----------------------------------------------*/

/* Private Functions ---------------------------------------------------------*/

/* Public Functions ----------------------------------------------------------*/
void SceneManager(uint32_t frame_count)
{
	uint16_t interval = 300;	// Duration of each scene before switching
	uint16_t time_in_scene;		// Time spent in the current scene

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
		// TODO: Implement glitch effect logic
		time_in_scene = frame_count % interval;
		break;
	case SCENE_MATRIX_FALLING:
		// TODO: Implement standard rain/falling character logic
		time_in_scene = frame_count % interval;
		break;
	default:
		break;
	}
}
