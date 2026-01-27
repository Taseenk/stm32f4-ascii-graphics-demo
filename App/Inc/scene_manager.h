/**
 ******************************************************************************
 * @file           : scene_manager.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __SCENE_MANAGER_H
#define __SCENE_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define SCENE_INTERVAL_FRAMES 		300		// Total duration of a scene in frames

// Glitch Scene
#define GLITCH_BRIGHTNESS_START 	0		// Frame index to start full brightness
#define GLITCH_DIM_START 			21		// Frame index to start medium green
#define GLITCH_FADE_START 			141		// Frame index to start dark green
#define GLITCH_NOISE_HIGH 			20		// Max character spawn rate (Full Brightness)
#define GLITCH_NOISE_MID 			4		// Reduced character spawn rate (Dimming)
#define GLITCH_NOISE_LOW 			2		// Minimum character spawn rate (Fade)
#define GLITCH_DISSOLVE_LIGHT 		5		// Chars removed per frame (Subtle dissolve)
#define GLITCH_DISSOLVE_HIGH 		15		// Chars removed per frame (Aggressive dissolve)
#define GLITCH_DISSOLVE_INTERVAL 	3		// Every 3th frame of scene

// Rain Scene
#define RAIN_DENSITY_STEPS 			40		// Frames to wait before switching density levels
#define RAIN_DENSITY_HIGH         	13    	// Most dense rain trail spawn rate
#define RAIN_DENSITY_MID          	6     	// Medium rain trail spawn rate
#define RAIN_DENSITY_LOW          	3     	// Sparse rain trail spawn rate

// Combined rain and glitch
#define RG_COLOR_BRIGHT_LIMIT     	40    	// Frame threshold for bright green
#define RG_COLOR_MEDIUM_LIMIT     	80    	// Frame threshold for medium green
#define RG_COLOR_DARK_LIMIT       	160   	// Frame threshold for dark green
#define RG_RAIN_TRANSITION        	(SCENE_INTERVAL_FRAMES / 2) // Mid-point for density swap
#define RG_DISSOLVE_INTERVAL 		3		// Every 3th frame of scene

/* typedefs ------------------------------------------------------------------*/
typedef enum {
	SCENE_MATRIX_GLITCH,
	SCENE_MATRIX_FALLING_GLITCH,
	SCENE_MATRIX_FALLING,
	SCENE_TOTAL_COUNT
} SceneID_t;

/* Function prototypes -------------------------------------------------------*/
void SceneManager(uint32_t frame_count);
void FPSDisplay(uint32_t fps_counter);

#endif /* __SCENE_MANAGER_H */