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
#define ASCII_PRINTABLE_START 33   // The '!' character
#define ASCII_PRINTABLE_RANGE 94   // Total printable characters up to '~'
#define COORDINATE_OFFSET     1    // Terminal coordinates usually start at 1,1
#define STRING_TERMINATOR   '\0'    // Null terminator for strings

/* typedefs ------------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale);

#endif /* __GFX_MATRIX_STREAM_H */