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
// Playlist of scenes to cycle through
static const SceneConfig_t scene_playlist[] = {
    {SCENE_MATRIX_GLITCH, GLITCH_DURATION, SCENE_TRANSITION_NONE},
    {SCENE_MATRIX_FALLING_GLITCH, RG_DURATION, SCENE_TRANSITION_NONE},
    {SCENE_MATRIX_FALLING, RAIN_DURATION, SCENE_TRANSITION_NONE},
};

static SceneState_t current_state = SCENE_STATE_START; // Initialize starting scene state
static uint8_t playlist_index = 0;                     // Current index in the scene playlist
static uint32_t scene_frame_counter = 0;               // Frame counter for the current scene

/* Private Function Prototypes -----------------------------------------------*/
void __RunActiveScene(SceneID_t id, uint32_t frame, uint32_t global_frame);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn void __RunActiveScene(SceneID_t id, uint32_t frame)
 * @brief Executes the logic for the currently active scene.
 * @param id The identifier of the active scene.
 * @param frame The current frame count used for timing within the scene.
 */
void __RunActiveScene(SceneID_t id, uint32_t frame, uint32_t global_frame)
{
	int step;

	switch (id) {
		/* Run the Matrix Glitch Scene */
		case SCENE_MATRIX_GLITCH:
			// Adjust text colour based on time in scene
			if (frame == GLITCH_BRIGHTNESS_START) {
				TerminalSetTextColour(FG_BRIGHT_GREEN);
			} else if (frame == GLITCH_DIM_START) {
				TerminalSetTextColour(FG_MEDIUM_GREEN);
			} else if (frame == GLITCH_FADE_START) {
				TerminalSetTextColour(FG_DARK_GREEN);
			}

			// Full Brightness
			if (frame < GLITCH_DIM_START) {
				// Spawn random characters
				MatrixCharacterNoise(global_frame, GLITCH_NOISE_HIGH, CHARACTER_ASCII_NOISE);

				// Occasional light dissolving to keep the screen from getting too crowded
				if (global_frame % GLITCH_DISSOLVE_INTERVAL == 0)
					MatrixCharacterDissolve(global_frame, GLITCH_DISSOLVE_LIGHT);
			}
			// Light Dimming
			else if (frame < GLITCH_FADE_START) {
				// Spawn fewer new characters, dissolve more existing ones
				MatrixCharacterNoise(global_frame, GLITCH_NOISE_MID, CHARACTER_ASCII_NOISE);
				MatrixCharacterDissolve(global_frame, GLITCH_DISSOLVE_HIGH);
			}
			// Deeper Fade
			else {
				// Very few new characters and keep dissolving
				MatrixCharacterNoise(global_frame, GLITCH_NOISE_LOW, CHARACTER_ASCII_NOISE);
				MatrixCharacterDissolve(global_frame, GLITCH_DISSOLVE_HIGH);
			}

			// break out of the switch
			break;

		/* Run the Falling Glitch Scene */
		case SCENE_MATRIX_FALLING_GLITCH:
			// Adjust text colour based on time in scene
			if (frame < RG_COLOR_BRIGHT_LIMIT) {
				TerminalSetTextColour(FG_BRIGHT_GREEN);
			} else if (frame < RG_COLOR_MEDIUM_LIMIT) {
				TerminalSetTextColour(FG_MEDIUM_GREEN);
			} else if (frame < RG_COLOR_DARK_LIMIT) {
				TerminalSetTextColour(FG_DARK_GREEN);
			} else {
				TerminalSetTextColour(FG_DARK_GREEN);
			}

			// Add a low density rain for the transition to the next scene
			MatrixRainUpdate(RAIN_DENSITY_LOW, RAIN_SPEED_LOW);

			// Disolving charachters at intervals
			if (frame % RG_DISSOLVE_INTERVAL == 0)
				MatrixCharacterDissolve(global_frame, GLITCH_DISSOLVE_HIGH);

			// break out of the switch
			break;

		/* Run the Falling Scene */
		case SCENE_MATRIX_FALLING:
			// Cycle through density steps every 40 frames
			step = (frame / RAIN_DENSITY_STEPS) % 3;

			switch (step) {
				case 0: // High Density
					TerminalSetTextColour(FG_BRIGHT_GREEN);
					MatrixRainUpdate(RAIN_DENSITY_HIGH, RAIN_SPEED_MID);
					break;

				case 1: // Medium Density
					TerminalSetTextColour(FG_MEDIUM_GREEN);
					MatrixRainUpdate(RAIN_DENSITY_MID, RAIN_SPEED_MID);
					break;

				case 2: // Low Density
					TerminalSetTextColour(FG_DARK_GREEN);
					MatrixRainUpdate(RAIN_DENSITY_LOW, RAIN_SPEED_LOW);
					break;
			}

			// break out of the switch
			break;

		default:
			// break out of the switch
			break;
	}
}

/* Public Functions ----------------------------------------------------------*/
/**
 * @fn void SceneManager(uint32_t global_frame_count)
 * @brief Manages scene transitions and execution based on the current state.
 * @param global_frame_count The global frame count since the program started.
 */
void SceneManager(uint32_t global_frame_count)
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
			__RunActiveScene(active_scene.id, scene_frame_counter, global_frame_count);

			// Check the duration of the scene to transition to EXIT state
			if (scene_frame_counter >= active_scene.duration) {
				current_state = SCENE_STATE_EXIT;
			} else {
				// Increment the scene frame counter
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
