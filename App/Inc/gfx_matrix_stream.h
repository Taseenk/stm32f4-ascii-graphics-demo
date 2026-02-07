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

#define TRAIL_LENGTH                    8      // Default trail length for matrix rain effect
#define MATRIX_RAIN_DENSITY_MASK        0x3FF   // Mask to normalize RNG for density comparison (0-1023) in matrix rain
#define COLUMN_INACTIVE                 0       // Indicator for inactive column

// Xorshift RNG Constants
#define XORSHIFT_S1                     13     // Shift constant 1
#define XORSHIFT_S2                     17     // Shift constant 2
#define XORSHIFT_S3                     5      // Shift constant 3

// Random Character Set Configuration
#define ASCII_PRINTABLE_START           33      // The '!' character
#define ASCII_CHAR_MASK                 0x3F    // Mask to get 64 printable ASCII characters
#define STRING_TERMINATOR               '\0'    // Null terminator for strings

/* Function prototypes -------------------------------------------------------*/
void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale);
void MatrixCharacterDissolve(uint32_t frame, uint8_t density_scale);

void MatrixRainUpdate(uint8_t density, uint8_t speed);

#endif /* __GFX_MATRIX_STREAM_H */