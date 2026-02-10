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
#include "rng_util.h"
#include "serial_hw.h"
#include "terminal.h"

// Standard libraries
#include <stdint.h>
#include <stdio.h>

/* Private Variables ---------------------------------------------------------*/
// Playlist of scenes to cycle through
static const SceneConfig_t scene_playlist[] = {
    {SCENE_INTRO_SCROLL, 300, SCENE_TRANSITION_CLEAR},
    {SCENE_VISUAL_DEMO, 240, SCENE_TRANSITION_CLEAR},
    {SCENE_ASCII_GLITCH_NOISE, ASCII_GLITCH_NOISE_DURATION, SCENE_TRANSITION_CLEAR},
    {SCENE_RAIN_FADE_IN, RAIN_FADE_IN_DURATION, SCENE_TRANSITION_NONE},
    {SCENE_MATRIX_RAIN, MATRIX_RAIN_DURATION, SCENE_TRANSITION_NONE},
    {SCENE_MATRIX_RAIN_HACKED, RAIN_HACKED_DURATION, SCENE_TRANSITION_CLEAR},
    {SCENE_BINARY_GLITCH_NOISE, BINARY_GLITCH_NOISE_DURATION, SCENE_TRANSITION_CLEAR},
};

static SceneState_t current_state = SCENE_STATE_START; // Initialize starting scene state
static uint8_t playlist_index = 0;                     // Current index in the scene playlist
static uint32_t scene_frame_counter = 0;               // Frame counter for the current scene

/* Private Function Prototypes -----------------------------------------------*/
void __RunActiveScene(SceneID_t id, uint32_t frame, uint32_t global_frame);
void __SceneIntroScroll(uint32_t frame, uint32_t global_frame);
void __SceneVisualDemo(uint32_t frame, uint32_t global_frame);
void __SceneAsciiGlitchNoise(uint32_t frame, uint32_t global_frame);
void __SceneRainFadeIn(uint32_t frame, uint32_t global_frame);
void __SceneMatrixRain(uint32_t frame);
void __SceneMatrixRainHacked(uint32_t frame);
void __SceneBinaryGlitchNoise(uint32_t frame, uint32_t global_frame);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn void __RunActiveScene(SceneID_t id, uint32_t frame)
 * @brief Executes the logic for the currently active scene.
 * @param id The identifier of the active scene.
 * @param frame The current frame count used for timing within the scene.
 */
void __RunActiveScene(SceneID_t id, uint32_t frame, uint32_t global_frame)
{
	switch (id) {
		case SCENE_ASCII_GLITCH_NOISE:
			__SceneAsciiGlitchNoise(frame, global_frame);
			// break out of the switch
			break;

		case SCENE_RAIN_FADE_IN:
			__SceneRainFadeIn(frame, global_frame);
			// break out of the switch
			break;

		case SCENE_MATRIX_RAIN:
			__SceneMatrixRain(frame);
			// break out of the switch
			break;

		case SCENE_MATRIX_RAIN_HACKED:
			__SceneMatrixRainHacked(frame);
			// break out of the switch
			break;

		case SCENE_BINARY_GLITCH_NOISE:
			__SceneBinaryGlitchNoise(frame, global_frame);
			// break out of the switch
			break;

		default:
			// break out of the switch
			break;
	}
}

/**
 * @fn void __SceneAsciiGlitchNoise(uint32_t frame, uint32_t global_frame)
 * @brief Scene logic for the ASCII Glitch Noise scene, which spawns random ASCII
 * characters and dissolves them over time.
 * @param frame The current frame count used for timing within the scene.
 * @param global_frame_count The global frame count since the program started.
 */
void __SceneAsciiGlitchNoise(uint32_t frame, uint32_t global_frame)
{
	// Adjust text colour based on time in scene to create a dynamic visual effect
	if (frame == GLITCH_SCENE_START)
		TerminalSetTextColour(FG_BRIGHT_GREEN);
	else if (frame == GLITCH_SCENE_BRIGHT)
		TerminalSetTextColour(FG_MEDIUM_GREEN);
	else if (frame == GLITCH_SCENE_DIM)
		TerminalSetTextColour(FG_DARK_GREEN);

	// Full Brightness
	if (frame < GLITCH_SCENE_BRIGHT) {
		// Spawn random characters
		MatrixCharacterNoise(global_frame, GLITCH_DENSITY_HIGH, CHARACTER_ASCII_NOISE);

		// Occasional light dissolving to keep the screen from getting too crowded
		if ((global_frame & DISSOLVE_INTERVAL_MASK) == 0)
			MatrixCharacterDissolve(global_frame, DISSOLVE_STRENGTH_LOW);
	}
	// Light Dimming
	else if (frame < GLITCH_SCENE_DIM) {
		// Spawn fewer new characters, dissolve more existing ones
		MatrixCharacterNoise(global_frame, GLITCH_DENSITY_MEDIUM, CHARACTER_ASCII_NOISE);
		MatrixCharacterDissolve(global_frame, DISSOLVE_STRENGTH_HIGH);
	}
	// Deeper Fade
	else {
		// Very few new characters and keep dissolving
		MatrixCharacterNoise(global_frame, GLITCH_DENSITY_LOW, CHARACTER_ASCII_NOISE);
		MatrixCharacterDissolve(global_frame, DISSOLVE_STRENGTH_HIGH);
	}
}

/**
 * @fn void __SceneRainFadeIn(uint32_t frame)
 * @brief Scene logic for the Rain Fade-In scene, which creates a gradual fade-in
 * effect for the Matrix rain by adjusting text colour and rain density over time.
 * @param frame The current frame count used for timing within the scene.
 * @param global_frame_count The global frame count since the program started.
 */
void __SceneRainFadeIn(uint32_t frame, uint32_t global_frame)
{
	// Adjust text colour based on time in scene
	if (frame < RAIN_FADE_IN_SCENE_BRIGHT) {
		TerminalSetTextColour(FG_BRIGHT_GREEN);
	} else if (frame < RAIN_FADE_IN_SCENE_MEDIUM) {
		TerminalSetTextColour(FG_MEDIUM_GREEN);
	} else if (frame < RAIN_FADE_IN_SCENE_DARK) {
		TerminalSetTextColour(FG_DARK_GREEN);
	}

	// Spawn rain trails with increasing density as the scene progresses
	if (frame < RAIN_FADE_IN_MIDPOINT)
		MatrixRainUpdate(RAIN_DENSITY_LOW, RAIN_SPEED_DEFAULT);
	else
		MatrixRainUpdate(RAIN_DENSITY_HIGH, RAIN_SPEED_DEFAULT);

	// Disolving charachters at intervals
	if (frame % DISSOLVE_INTERVAL_MASK == 0)
		MatrixCharacterDissolve(global_frame, DISSOLVE_STRENGTH_HIGH);
}

/**
 * @fn void __SceneMatrixRain(uint32_t frame)
 * @brief Scene logic for the Matrix Falling Rain scene, which creates falling
 * rain trails and periodically dissolves characters to prevent overcrowding.
 * @param frame The current frame count used for timing within the scene.
 */
void __SceneMatrixRain(uint32_t frame)
{
	// Cycle through text colours to create a dynamic visual effect
	uint32_t color_cycle = (frame / TERMINAL_HEIGHT) % RAIN_COLOR_STAGES;
	if (color_cycle == 0)
		TerminalSetTextColour(FG_BRIGHT_GREEN);
	else if (color_cycle == 1)
		TerminalSetTextColour(FG_MEDIUM_GREEN);
	else
		TerminalSetTextColour(FG_DARK_GREEN);

	// Create falling rain trails
	MatrixRainUpdate(RAIN_DENSITY_HIGH, RAIN_SPEED_DEFAULT);

	// Occasional light dissolving to keep the screen from getting too crowded
	if ((frame & DISSOLVE_INTERVAL_MASK) == 0)
		MatrixCharacterDissolve(frame, DISSOLVE_STRENGTH_LOW);
}

/**
 * @fn void __SceneMatrixRainHacked(uint32_t frame)
 * @brief Scene logic for the Matrix Falling Rain Hacked scene, which introduces
 * red "corrupted" characters as the scene progresses to simulate a system hack.
 * @param frame The current frame count used for timing within the scene.
 */
void __SceneMatrixRainHacked(uint32_t frame)
{
	// Variables for color cycling and random number generation
	static uint32_t rand_num;
	uint32_t color_cycle = (frame / TERMINAL_HEIGHT) % RAIN_COLOR_STAGES;

	// Initialize random number on first frame
	if (frame == 0)
		rand_num = GetRandomNumber();

	// System has been hacked/taken over data is corrupt (text becomes red)
	if (frame > RAIN_HACKED_CORRUPTED) {
		// Cycle through red tones instead of green
		if (color_cycle == 0)
			TerminalSetTextColour(FG_BRIGHT_RED);
		else if (color_cycle == 1)
			TerminalSetTextColour(FG_MEDIUM_RED);
		else
			TerminalSetTextColour(FG_DARK_RED);
	} else {
		// Update the random number using Xorshift algorithm
		XorshiftRandomNumber(&rand_num);

		// Bitwise mask to create a small chance of red characters appearing (getting hacked)
		if ((rand_num & RAIN_HACKED_CORRUPT_MASK) < 7) {
			TerminalSetTextColour(FG_MEDIUM_RED);
		} else {
			// Initial normal green fade-in of the rain before the "hack" takes over
			if (color_cycle == 0) {
				TerminalSetTextColour(FG_BRIGHT_GREEN);
			} else if (color_cycle == 1)
				TerminalSetTextColour(FG_MEDIUM_GREEN);
			else
				TerminalSetTextColour(FG_DARK_GREEN);
		}
	}

	// Create falling rain trails
	MatrixRainUpdate(RAIN_DENSITY_HIGH, RAIN_SPEED_DEFAULT);

	// Occasional light dissolving to keep the screen from getting too crowded
	if ((frame & DISSOLVE_INTERVAL_MASK) == 0)
		MatrixCharacterDissolve(frame, DISSOLVE_STRENGTH_LOW);
}

/**
 * @fn void __SceneBinaryGlitchNoise(uint32_t frame, uint32_t global_frame_count)
 * @brief Scene logic for the Binary Glitch Noise scene, which spawns random binary
 * characters and dissolves them over time.
 * @param frame The current frame count used for timing within the scene.
 * @param global_frame_count The global frame count since the program started.
 */
void __SceneBinaryGlitchNoise(uint32_t frame, uint32_t global_frame)
{
	// Adjust text colour based on time in scene to create a dynamic visual effect
	if (frame == GLITCH_SCENE_START)
		TerminalSetTextColour(FG_BRIGHT_GREEN);
	else if (frame == GLITCH_SCENE_BRIGHT)
		TerminalSetTextColour(FG_MEDIUM_GREEN);
	else if (frame == GLITCH_SCENE_DIM)
		TerminalSetTextColour(FG_DARK_GREEN);

	// Full Brightness
	if (frame < GLITCH_SCENE_BRIGHT) {
		// Spawn random characters
		MatrixCharacterNoise(global_frame, GLITCH_DENSITY_MEDIUM, CHARACTER_BINARY_NOISE);

		// Occasional light dissolving to keep the screen from getting too crowded
		if ((global_frame & DISSOLVE_INTERVAL_MASK) == 0)
			MatrixCharacterDissolve(global_frame, DISSOLVE_STRENGTH_LOW);
	}
	// Light Dimming
	else if (frame < GLITCH_SCENE_DIM) {
		// Spawn fewer new characters, dissolve more existing ones
		MatrixCharacterNoise(global_frame, GLITCH_DENSITY_MEDIUM, CHARACTER_BINARY_NOISE);
		MatrixCharacterDissolve(global_frame, DISSOLVE_STRENGTH_HIGH);
	}
	// Deeper Fade
	else {
		// Very few new characters and keep dissolving
		MatrixCharacterNoise(global_frame, GLITCH_DENSITY_LOW, CHARACTER_BINARY_NOISE);
		MatrixCharacterDissolve(global_frame, DISSOLVE_STRENGTH_HIGH);
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
			playlist_index = (playlist_index + 1) % SCENE_TOTAL_SCENES;

			// Move back to START state for the next scene
			current_state = SCENE_STATE_START;
			break;

		/* Default case to catch unexpected states */
		default:
			break;
	}
}
