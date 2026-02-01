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
#include <stdint.h>
#include <stdio.h>

/* Private Variables ---------------------------------------------------------*/
static const SceneConfig_t scene_playlist[] = {
    {SCENE_MATRIX_GLITCH, 20, SCENE_TRANSITION_NONE},
    {SCENE_MATRIX_FALLING_GLITCH, 10, SCENE_TRANSITION_NONE},
    {SCENE_MATRIX_FALLING, 10, SCENE_TRANSITION_NONE},
};

static SceneState_t current_state = SCENE_STATE_START; // Starting state
// SceneID_t current_scene_id = SCENE_MATRIX_GLITCH; 	// Starting scene
static uint8_t playlist_index = 0;
static uint32_t scene_frame_counter = 0; // Frame counter for the current scene

/* Private Function Prototypes -----------------------------------------------*/
void __RunActiveScene(SceneID_t id, uint32_t frame);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn void __RunActiveScene(SceneID_t id, uint32_t frame)
 * @brief Executes the logic for the currently active scene.
 * @param id The identifier of the active scene.
 * @param frame The current frame count used for timing within the scene.
 */
void __RunActiveScene(SceneID_t id, uint32_t frame)
{
	switch (id) {
		/* Run the Matrix Glitch Scene */
		case SCENE_MATRIX_GLITCH:
			// (Implementation of the scene logic goes here)
			break;

		/* Run the Falling Glitch Scene */
		case SCENE_MATRIX_FALLING_GLITCH:
			// (Implementation of the scene logic goes here)
			break;

		/* Run the Falling Scene */
		case SCENE_MATRIX_FALLING:
			// (Implementation of the scene logic goes here)
			break;

		default:
			break;
	}
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void SceneManager(uint32_t frame_count)
 * @brief Manages scene transitions and executes the current scene logic.
 * @param frame_count The global frame count used for timing scene transitions.
 */
void SceneManager(uint32_t frame_count)
{
	// Get the currently active scene
	SceneConfig_t active_scene = scene_playlist[playlist_index];

	switch (current_state) {
		/* Initialize scenes */
		case SCENE_STATE_START:
			// Reset the terminal display for the new scene
			if (active_scene.screen_transition == SCENE_TRANSITION_CLEAR) {
				TerminalClearAndHome();
			}

			// Reset scene frame counter and continue to RUN state
			scene_frame_counter = 0;
			current_state = SCENE_STATE_RUN;
			break;

		/* Run scene logic */
		case SCENE_STATE_RUN:
			// Run the active scene logic
			__RunActiveScene(active_scene.id, scene_frame_counter);

			// Check the duration of the scene to transition to EXIT state
			if (scene_frame_counter >= active_scene.duration) {
				current_state = SCENE_STATE_EXIT;
			} else {
				// Increment scene frame counter keeping up scene managment
				scene_frame_counter++;
			}

			break;

		/* Cleanup and transition to next scene */
		case SCENE_STATE_EXIT:
			// Reset terminal styling
			TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

			// Move to next playlist index, wrapping back to 0 at the end of the list
			playlist_index = (playlist_index + 1) % SCENE_PLAYLIST_SIZE;

			// Move back to START state for the next scene
			current_state = SCENE_STATE_START;
			break;

		/* Default case to catch unexpected states */
		default:
			break;
	}
}

/**
 * @fn void FPSDisplay(uint32_t fps_counter)
 * @brief Displays the current FPS on the terminal for debugging purposes.
 * @param fps_counter The current calculated frames per second.
 */
void FPSDisplay(uint32_t fps_counter)
{
	// Current calculated FPS value
	uint32_t fps = fps_counter;

	// Buffer for UART/Terminal debug strings
	char debug_msg[64];

	// Output FPS info to the top-left of the terminal
	// TerminalSetTextColour(FG_BLACK);
	TerminalSetCursorPos(1, 1);
	sprintf(debug_msg, "FPS: %-3lu", (unsigned long)fps);
	SerialPrint(debug_msg);

	// Restore the default terminal styling
	// TerminalSetColour(FG_DEFAULT, BG_DEFAULT);
}
