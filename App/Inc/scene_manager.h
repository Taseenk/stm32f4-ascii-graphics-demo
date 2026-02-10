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
// Scene Durations (in frames)
#define SCENE_DEFAULT_DURATION 			300
#define ASCII_GLITCH_NOISE_DURATION		150
#define RAIN_FADE_IN_DURATION			300
#define MATRIX_RAIN_DURATION			350
#define RAIN_HACKED_DURATION			350
#define BINARY_GLITCH_NOISE_DURATION	150

// Dissolve Settings
#define DISSOLVE_INTERVAL_MASK 		0x04	// Used for bitwise frame frequency (frame & 4)
#define DISSOLVE_STRENGTH_LOW	    5		// Characters removed per frame (Subtle dissolve)
#define DISSOLVE_STRENGTH_HIGH 		15		// Characters removed per frame (Aggressive dissolve)

// Charachter/Binary random glitch
#define GLITCH_SCENE_START			0		// Scene frame index to start full brightness
#define GLITCH_SCENE_BRIGHT			21		// Scene frame index to start medium green
#define GLITCH_SCENE_DIM			75		// Scene frame index to start dark green

#define GLITCH_DENSITY_HIGH   		20		// Max character spawn rate
#define GLITCH_DENSITY_MEDIUM 		4		// Reduced character spawn rate
#define GLITCH_DENSITY_LOW    		2		// Low character spawn rate

// Matrix falling Rain Scene
#define RAIN_COLOR_STAGES      		3		// Number of color stages for a dynamic visual effect
#define RAIN_DENSITY_HIGH         	13    	// Most dense rain trail spawn rate
#define RAIN_DENSITY_MID          	6     	// Medium rain trail spawn rate
#define RAIN_DENSITY_LOW          	3     	// Sparse rain trail spawn rate

#define RAIN_SPEED_DEFAULT			1		// Number of rows characters move per update at low speed

// Hacked Falling rain scene
#define RAIN_HACKED_CORRUPTED       265     // Scene frame index when the "hack" takes full effect and characters become red
#define RAIN_HACKED_CORRUPT_MASK    0x3ff   // Bitwise mask to create a small chance of red characters appearing (getting hacked)

// Rain Fade-In Scene
#define RAIN_FADE_IN_SCENE_BRIGHT    40
#define RAIN_FADE_IN_SCENE_MEDIUM    80
#define RAIN_FADE_IN_SCENE_DARK      160

#define RAIN_FADE_IN_MIDPOINT      	(RAIN_FADE_IN_DURATION / 2) // Mid-point for density swap

/* typedefs ------------------------------------------------------------------*/
// Scene Identifiers
typedef enum {
    SCENE_INTRO_SCROLL,         // Intro screen with scrolling text
    SCENE_VISUAL_DEMO,          // Terminal capabilities (styles & graphics)
	SCENE_ASCII_GLITCH_NOISE,   // Random ASCII sporadic glitch
	SCENE_RAIN_FADE_IN,			// Transitional scene to Matrix falling rain effect
    SCENE_MATRIX_RAIN,          // Matrix falling rain effect
    SCENE_MATRIX_RAIN_HACKED,   // Matrix falling rain effect with data corruption (hacked)
    SCENE_BINARY_GLITCH_NOISE,	// Random binary (0/1) sporadic glitch
    SCENE_TOTAL_SCENES			// Total number of scenes in the playlist
} SceneID_t;

// Scene Transition Types
typedef enum {
    SCENE_TRANSITION_NONE,		// Instant switch with no visual effect
    SCENE_TRANSITION_CLEAR,		// Clear the screen and buffer before next scene
    SCENE_TOTAL_TRANSITIONS		// Total number of available transition types
} SceneTransition_t;

// Scene States
typedef enum {
	SCENE_STATE_START,			// Initialize scenes and variables
	SCENE_STATE_RUN,			// Main scene execution
	SCENE_STATE_EXIT,			// Cleanup and prepare for next scene
    SCENE_TOTAL_STATES			// Total number of available states
} SceneState_t;

// Scene Configuration Structure
typedef struct {
	SceneID_t id;							// Scene identifier
	uint32_t duration;						// Duration of the scene in frames
    SceneTransition_t screen_transition;	// Type of screen transition effect
} SceneConfig_t;


/* Function prototypes -------------------------------------------------------*/
void SceneManager(uint32_t frame_count);

#endif /* __SCENE_MANAGER_H */