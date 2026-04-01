/**
 ******************************************************************************
 * @file           : scene_manager.h
 * @brief          : Header file for the scene management system that handles
 * cycling through different ASCII art scenes on an STM32F4 microcontroller.
 * This module defines the structures, enumerations, and function prototypes
 * for managing scene transitions, timing, and state management for both automatic
 * cycling and user-defined playlists of scenes.
 ******************************************************************************
 */

#ifndef __SCENE_MANAGER_H
#define __SCENE_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
// Scene Durations (in frames)
#define SCENE_DEFAULT_DURATION 300 // Default duration in auto mode

/* typedefs ------------------------------------------------------------------*/
// Scene Identifiers
typedef enum {
	SCENE_ATTRIBUTES_DEMO,   // VT100 attribute torture test showcasing all text attributes and combinations
	SCENE_SMPTE_CALIBRATION, // SMPTE colour bar calibration pattern

	SCENE_RADIAL_GREYSCALE, // Radial gradient pattern using greyscale characters and colours
	SCENE_RADIAL_COLOUR,    // Radial gradient pattern using extended colour range without character shading for a block
	                        // colour effect

	SCENE_PRIMITIVES_DEMO, // Demo of drawing basic primitives like lines, rectangles, circles

	SCENE_ASCII_GLITCH_NOISE,  // Random ASCII sporadic glitch
	SCENE_BINARY_GLITCH_NOISE, // Random binary (0/1) sporadic glitch

	SCENE_ASCII_MATRIX_RAIN,  // ASCII Matrix falling rain effect
	SCENE_BINARY_MATRIX_RAIN, // Binary Matrix falling rain effect
	SCENE_MATRIX_RAIN_HACKED, // Matrix falling rain effect with data corruption (hacked)

	SCENE_RAIN_FADE_IN, // Transitional scene to Matrix falling rain effect

	SCENE_TOTAL_SCENES // Total number of available scenes
} SceneID_t;

// Scene Transition Types
typedef enum {
	SCENE_TRANSITION_NONE,  // Instant switch with no visual effect
	SCENE_TRANSITION_CLEAR, // Clear the screen and buffer before next scene
	SCENE_TOTAL_TRANSITIONS // Total number of available transition types
} SceneTransition_t;

// Scene Configuration Structure
typedef struct {
	SceneID_t id;                        // Scene identifier
	uint32_t duration;                   // Duration of the scene in frames
	SceneTransition_t screen_transition; // Type of screen transition effect
	void (*init)(void); // Optional initialization function for the scene, called once at the start of the scene
	void (*render)(uint32_t scene_frame); // Render function for the scene, called every frame with the current scene
	                                      // frame count for timing-based effects
} SceneConfig_t;

// Scene States
typedef enum {
	SCENE_STATE_START, // Initialize scenes and variables
	SCENE_STATE_RUN,   // Main scene execution
	SCENE_STATE_EXIT,  // Cleanup and prepare for next scene
	SCENE_TOTAL_STATES // Total number of available states
} SceneState_t;

/* Function prototypes -------------------------------------------------------*/
void SceneManagerInit(void);
void SceneManager(uint32_t frame_count);

#endif /* __SCENE_MANAGER_H */