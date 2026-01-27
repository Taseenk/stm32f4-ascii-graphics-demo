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