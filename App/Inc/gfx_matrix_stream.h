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
#define ASCII_PRINTABLE_START   33      // The '!' character
#define ASCII_CHAR_MASK         0x3F    // Mask to get 64 printable ASCII characters
#define DENSITY_MASK            0x7F    // Mask for density calculation
#define COORDINATE_OFFSET       1       // Terminal coordinates usually start at 1,1
#define STRING_TERMINATOR       '\0'    // Null terminator for strings
#define TRAIL_LENGTH            7       // Default trail length for matrix rain effect

/* typedefs ------------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale);
void MatrixCharacterDissolve(uint32_t frame, uint8_t density_scale);
void MatrixRainUpdate(uint32_t frame, uint8_t speed, uint8_t density);

#endif /* __GFX_MATRIX_STREAM_H */