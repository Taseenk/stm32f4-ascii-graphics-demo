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
#define SCENE_INTERVAL_FRAMES 300

// Glitch Scene
#define GLITCH_BRIGHTNESS_START 0
#define GLITCH_DIM_START 21
#define GLITCH_FADE_START 141
#define GLITCH_NOISE_HIGH 20
#define GLITCH_NOISE_MID 4
#define GLITCH_NOISE_LOW 2
#define GLITCH_DISSOLVE_LIGHT        5
#define GLITCH_DISSOLVE_HIGH         15

// Rain Scene
#define RAIN_DENSITY_STEPS 40
#define RAIN_DENSITY_HIGH 13
#define RAIN_DENSITY_MID 6
#define RAIN_DENSITY_LOW 3

/* typedefs ------------------------------------------------------------------*/
typedef enum {
	SCENE_MATRIX_GLITCH,
	SCENE_MATRIX_FALLING,
	SCENE_MATRIX_FALLING_GLITCH,
	SCENE_TOTAL_COUNT
} SceneID_t;

/* Function prototypes -------------------------------------------------------*/
void SceneManager(uint32_t frame_count);
void FPSDisplay(uint32_t fps_counter);

#endif /* __SCENE_MANAGER_H */