/**
 ******************************************************************************
 * @file           : scene_manager.c
 * @brief          : Scene management system for cycling through different ASCII
 * art scenes on an STM32F4 microcontroller. This module handles scene transitions,
 * timing, and state management for both automatic cycling and user-defined
 * playlists of scenes.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "scene_manager.h"
#include "terminal.h"

#include "scene_colour_demo.h"
#include "scene_glitch.h"
#include "scene_matrix_rain.h"

// STM32 libraries
#include "main.h"

/* Private Defines -----------------------------------------------------------*/
// Scene Durations (in frames)
#define SMPTE_CALIBRATION_DURATION 360
#define RADIAL_COLOUR_DURATION     250
#define GLITCH_NOISE_DURATION      150
#define MATRIX_RAIN_DURATION       350
#define RAIN_FADE_IN_DURATION      300

/* Private Variables ---------------------------------------------------------*/
// Table with configurations for all scenes
static const SceneConfig_t scene_table[] = {
    {SCENE_SMPTE_CALIBRATION, SMPTE_CALIBRATION_DURATION, SCENE_TRANSITION_CLEAR, ColourDemoInit,
     SmpteCalibrationRender},
    {SCENE_RADIAL_GREYSCALE, RADIAL_COLOUR_DURATION, SCENE_TRANSITION_CLEAR, ColourDemoInit, RadialGreyscaleRender},
    {SCENE_RADIAL_COLOUR, RADIAL_COLOUR_DURATION, SCENE_TRANSITION_CLEAR, ColourDemoInit, RadialColourRender},

    {SCENE_ASCII_GLITCH_NOISE, GLITCH_NOISE_DURATION, SCENE_TRANSITION_CLEAR, GlitchInit, AsciiGlitchRender},
    {SCENE_BINARY_GLITCH_NOISE, GLITCH_NOISE_DURATION, SCENE_TRANSITION_CLEAR, GlitchInit, BinaryGlitchRender},

    {SCENE_ASCII_MATRIX_RAIN, MATRIX_RAIN_DURATION, SCENE_TRANSITION_NONE, MatrixRainInit, AsciiRainRender},
    {SCENE_BINARY_MATRIX_RAIN, MATRIX_RAIN_DURATION, SCENE_TRANSITION_NONE, MatrixRainInit, BinaryRainRender},
    {SCENE_MATRIX_RAIN_HACKED, MATRIX_RAIN_DURATION, SCENE_TRANSITION_CLEAR, MatrixRainInit, AsciiRainHackedRender},

    {SCENE_RAIN_FADE_IN, RAIN_FADE_IN_DURATION, SCENE_TRANSITION_NONE, MatrixRainInit, AsciiRainFadeIn},
};
const uint8_t scene_table_count = sizeof(scene_table) / sizeof(scene_table[0]);

// Playlist of scenes to cycle through in playlist mode and total count of scenes in the playlist
static const SceneID_t scene_playlist[] = {
    SCENE_SMPTE_CALIBRATION, SCENE_RADIAL_GREYSCALE,  SCENE_ASCII_GLITCH_NOISE,
    SCENE_RAIN_FADE_IN,      SCENE_ASCII_MATRIX_RAIN, SCENE_MATRIX_RAIN_HACKED,
};
const uint8_t scene_playlist_count = sizeof(scene_playlist) / sizeof(scene_playlist[0]);

// State variables for scene management
static SceneState_t current_state = SCENE_STATE_START; // Initialize starting scene state
static uint8_t scene_index = 0;                        // Current index in the scene table
static uint32_t scene_frame_counter = 0;               // Frame counter for the current scene

/* Private Function Prototypes -----------------------------------------------*/
static uint8_t __GetTotalIndexCount(void);
static const SceneConfig_t *__GetActiveScene(void);
static uint32_t __GetSceneDuration(uint32_t scene_duration);

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn static uint8_t __GetTotalIndexCount(void)
 * @brief Determines the total number of scenes available based
 * on the current system mode (auto or playlist).
 * @return Returns the total count of scenes to cycle through in the current mode.
 */
static uint8_t __GetTotalIndexCount(void)
{
	// In auto mode, return the total number of scenes in the scene table
	if (system_mode == SYSTEM_STATE_AUTO_SCENE)
	{
		return (uint8_t)scene_table_count;
	}

	// In playlist mode, return the total number of scenes defined in the playlist
	if (system_mode == SYSTEM_STATE_PLAYLIST_SCENE)
	{
		return (uint8_t)scene_playlist_count;
	}

	// Default return value if system mode is not recognized
	return FALSE;
}

/**
 * @fn static const SceneConfig_t *__GetActiveScene(void)
 * @brief Retrieves the configuration of the currently active scene based on the system mode (auto or playlist).
 * In auto mode, it returns the current scene from the scene table based on the scene index. In playlist mode,
 * it searches for the current target scene in the playlist and returns its configuration from the scene table.
 * @return A pointer to the SceneConfig_t structure of the active scene, or NULL if no valid scene is found.
 */
static const SceneConfig_t *__GetActiveScene(void)
{
	// Return the current active scene configuration based on the system mode
	// System mode is auto
	if (system_mode == SYSTEM_STATE_AUTO_SCENE)
	{
		// Early exit if scene_index index is out of bouds
		if (scene_index >= scene_table_count)
			return NULL;

		// Return the current active scene configuration from the scene table based on the current index
		return &scene_table[scene_index];
	}

	// System mode is playlist
	if (system_mode == SYSTEM_STATE_PLAYLIST_SCENE)
	{
		// Early exit if scene_index index is out of bouds
		if (scene_index >= scene_playlist_count)
			return NULL;

		// Get the target scene ID from the playlist based on the current index
		SceneID_t target_scene = scene_playlist[scene_index];

		// Search through all scenes in the scene table to find the configuration for the current target scene in the
		// playlist
		for (uint8_t i = 0; i < scene_table_count; i++)
		{
			if (scene_table[i].id == target_scene)
				return &scene_table[i];
		}
	}

	// Default return for unrecognized mode or no match found
	return NULL;
}

/**
 * @fn static uint32_t __GetSceneDuration(uint32_t scene_duration)
 * @brief Determines the duration of the current scene based on the system mode.
 * In auto mode, it returns a default duration for all scenes. In playlist mode,
 * it returns the specified duration for the scene.
 * @param scene_duration The specified duration for the scene in frames.
 * @return The duration to use for the current scene in frames.
 */
static uint32_t __GetSceneDuration(uint32_t scene_duration)
{
	// In auto mode use default scene duration for all scenes
	if (system_mode == SYSTEM_STATE_AUTO_SCENE)
		return SCENE_DEFAULT_DURATION;

	// In playlist mode use the excisting specified duration for the scene
	return scene_duration;
}

/* Private Functions ---------------------------------------------------------*/
/**
 * @fn void SceneManager(uint32_t global_frame_count)
 * @brief
 */
void SceneManagerInit(void)
{
	// Reset to the first scene and initialize state
	current_state = SCENE_STATE_START;
	scene_index = 0;
	scene_frame_counter = 0;
}

void SceneManager(uint32_t global_frame_count)
{
	// Get the currently active scene
	const SceneConfig_t *active_scene = __GetActiveScene();

	// If no valid active scene is found, reset to the first scene and return
	if (active_scene == NULL)
	{
		SceneManagerInit();
		return;
	}

	// Manage scenes based on states start, run, and exit
	switch (current_state)
	{
		/* Initialize scenes, apply transition, run init, reset counter */
		case SCENE_STATE_START:
			// Always reset and clear the terminal for the new scene in auto mode
			// In playlist mode reset and clear the terminal based on screen transition type
			if ((active_scene->screen_transition == SCENE_TRANSITION_CLEAR &&
			     system_mode == SYSTEM_STATE_PLAYLIST_SCENE) ||
			    system_mode == SYSTEM_STATE_AUTO_SCENE)
			{
				TerminalClearAndHome();
			}

			// Run the init function for the scene if it exists
			if (active_scene->init != NULL)
				active_scene->init();

			// Reset scene frame counter and continue to RUN state
			scene_frame_counter = 0;
			current_state = SCENE_STATE_RUN;
			break;

		/* Run scene logic checking duration based on auto or playlist mode */
		case SCENE_STATE_RUN:
			// Check the duration of the scene to transition to EXIT state
			if (scene_frame_counter >= __GetSceneDuration(active_scene->duration))
			{
				current_state = SCENE_STATE_EXIT;
				break;
			}
			if (active_scene->render != NULL)
				active_scene->render(scene_frame_counter);

			// Increment the scene frame counter
			scene_frame_counter++;
			break;

		/* Cleanup and transition to next scene or return to start */
		case SCENE_STATE_EXIT:
			// Reset terminal styling
			TerminalSetColour(FG_DEFAULT, BG_DEFAULT);

			// Move to next playlist index, wrapping back to 0 at the end of the list
			scene_index = (scene_index + 1) % __GetTotalIndexCount();

			// Move back to START state for the next scene
			current_state = SCENE_STATE_START;
			break;

		/* Default case to catch unexpected states */
		default:
			break;
	}
}