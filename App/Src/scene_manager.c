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
SceneID_t current_scene = SCENE_MATRIX_FALLING;

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
	uint16_t time_in_scene; // Time spent in the current scene

	switch (id) {
		/* Run the Matrix Glitch Scene */
		case SCENE_MATRIX_GLITCH:
			// Determine time spent in the current scene
			time_in_scene = frame % SCENE_INTERVAL_FRAMES;

			// Adjust text colour based on time in scene
			if (time_in_scene == GLITCH_BRIGHTNESS_START) {
				TerminalSetTextColour(FG_BRIGHT_GREEN);
			} else if (time_in_scene == GLITCH_DIM_START) {
				TerminalSetTextColour(FG_MEDIUM_GREEN);
			} else if (time_in_scene == GLITCH_FADE_START) {
				TerminalSetTextColour(FG_DARK_GREEN);
			}

			// Full Brightness
			if (time_in_scene < GLITCH_DIM_START) {
				// Spawn random characters
				MatrixCharacterNoise(frame, GLITCH_NOISE_HIGH);

				// Occasional light dissolving to keep the screen from getting too crowded
				if (frame % GLITCH_DISSOLVE_INTERVAL == 0)
					MatrixCharacterDissolve(frame, GLITCH_DISSOLVE_LIGHT);
			}
			// Light Dimming
			else if (time_in_scene < GLITCH_FADE_START) {
				// Spawn fewer new characters, dissolve more existing ones
				MatrixCharacterNoise(frame, GLITCH_NOISE_MID);
				MatrixCharacterDissolve(frame, GLITCH_DISSOLVE_HIGH);
			}
			// Deeper Fade
			else {
				// Very few new characters and keep dissolving
				MatrixCharacterNoise(frame, GLITCH_NOISE_LOW);
				MatrixCharacterDissolve(frame, GLITCH_DISSOLVE_HIGH);
			}
			break;

		/* Run the Falling Glitch Scene */
		case SCENE_MATRIX_FALLING_GLITCH:
			// Determine time spent in the current scene
			time_in_scene = frame % SCENE_INTERVAL_FRAMES;

			// Adjust text colour based on time in scene
			if (time_in_scene < RG_COLOR_BRIGHT_LIMIT) {
				TerminalSetTextColour(FG_BRIGHT_GREEN);
			} else if (time_in_scene < RG_COLOR_MEDIUM_LIMIT) {
				TerminalSetTextColour(FG_MEDIUM_GREEN);
			} else if (time_in_scene < RG_COLOR_DARK_LIMIT) {
				TerminalSetTextColour(FG_DARK_GREEN);
			} else {
				TerminalSetTextColour(FG_DARK_GREEN);
			}

			// Add a low density rain for the transition to the next scene
			MatrixRainUpdate(frame, RAIN_DENSITY_LOW);

			// Add glitch effects at intervals
			if (time_in_scene % RG_DISSOLVE_INTERVAL == 0)
				MatrixCharacterDissolve(frame, GLITCH_DISSOLVE_HIGH);
			break;

		/* Run the Falling Scene */
		case SCENE_MATRIX_FALLING:
			// Determine time spent in the current scene
			time_in_scene = frame % SCENE_INTERVAL_FRAMES;

			// Cycle through density steps every 40 frames
			int step = (time_in_scene / RAIN_DENSITY_STEPS) % 3;

			switch (step) {
				case 0: // High Density
					TerminalSetTextColour(FG_BRIGHT_GREEN);
					MatrixRainUpdate(frame, RAIN_DENSITY_HIGH);
					break;

				case 1: // Medium Density
					TerminalSetTextColour(FG_MEDIUM_GREEN);
					MatrixRainUpdate(frame, RAIN_DENSITY_MID);
					break;

				case 2: // Low Density
					TerminalSetTextColour(FG_DARK_GREEN);
					MatrixRainUpdate(frame, RAIN_DENSITY_LOW);
					break;
			}
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
	// Scene transition at the end of every interval
	if (frame_count % SCENE_INTERVAL_FRAMES == 0) {
		// Move to next scene index, wrapping back to 0 at the end of the list
		current_scene = (SceneID_t)((current_scene + 1) % SCENE_TOTAL_SCENES);

		// Reset the terminal display for the new scene
		// TerminalClearAndHome();
	}

	// Scene-specific logic
	__RunActiveScene(current_scene, frame_count);
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
