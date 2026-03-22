/**
 ******************************************************************************
 * @file           : scene_matrix_rain.h
 * @brief          : Header file for the Matrix Rain scene, defining the 
 * initialization and rendering functions for the classic "Matrix" rain effect, 
 * including variations for ASCII, Binary, "Hacked" (red) corruption, and 
 * fade-in transitions.
 ******************************************************************************
 */

#ifndef __SCENE_MATRIX_RAIN_H
#define __SCENE_MATRIX_RAIN_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void MatrixRainInit(void);
void AsciiRainRender(uint32_t scene_frame);
void BinaryRainRender(uint32_t scene_frame);
void AsciiRainHackedRender(uint32_t scene_frame);
void AsciiRainFadeIn(uint32_t scene_frame);

#endif /* __SCENE_MATRIX_RAIN_H */