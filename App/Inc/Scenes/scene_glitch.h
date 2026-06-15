/**
 ******************************************************************************
 * @file           : scene_glitch.h
 * @brief          : Header file for the glitch noise animation scenes.
 * This module exports the initialization and frame-rendering functions for both
 * ASCII-based and binary-based sporadic glitch effects within the scene manager.
 ******************************************************************************
 */

#ifndef __SCENE_GLITCH_H
#define __SCENE_GLITCH_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void GlitchInit(void);
void AsciiGlitchRender(uint32_t scene_frame);
void BinaryGlitchRender(uint32_t scene_frame);

#endif /* __SCENE_GLITCH_H */