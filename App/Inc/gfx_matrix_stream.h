/**
 ******************************************************************************
 * @file           : gfx_matrix_stream.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __GFX_MATRIX_STREAM_H
#define __GFX_MATRIX_STREAM_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
// Animation Behavior & Timing
#define COORDINATE_OFFSET               1       // Terminal coordinates usually start at 1,1

#define TRAIL_LENGTH                    8       // Default trail length for matrix rain effect
#define MATRIX_RAIN_DENSITY_MASK        0x3FF   // Mask to normalize RNG for density comparison (0-1023) in matrix rain
#define COLUMN_INACTIVE                 0       // Indicator for inactive column

/* Function prototypes -------------------------------------------------------*/
void MatrixRainUpdate(uint8_t density, uint8_t speed);

#endif /* __GFX_MATRIX_STREAM_H */