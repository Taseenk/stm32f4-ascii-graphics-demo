/**
 ******************************************************************************
 * @file           : scene_primitives_demo.h
 * @brief          : Header file for the geometric primitives demonstration scenes.
 * This module declares the initialization and rendering interfaces for the
 * structural vector-art test scenes managed by the system execution loop.
 ******************************************************************************
 */

#ifndef __SCENE_PRIMITIVES_DEMO_H
#define __SCENE_PRIMITIVES_DEMO_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void ScenePrimitivesInit(void);
void ScenePrimitivesRender(uint32_t scene_frame);

#endif /* __SCENE_PRIMITIVES_DEMO_H */