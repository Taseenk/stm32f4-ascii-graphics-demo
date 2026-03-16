/**
 ******************************************************************************
 * @file           : scene_matrix_rain.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __SCENE_MATRIX_RAIN_H
#define __SCENE_MATRIX_RAIN_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void MatrixRainInit(void);
void AsciiMatrixRainRender(uint32_t scene_frame);
void BinaryMatrixRainRender(uint32_t scene_frame);
void MatrixRainHackedRender(uint32_t scene_frame);

#endif /* __SCENE_MATRIX_RAIN_H */