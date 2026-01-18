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

#define FG_BRIGHT_GREEN "\x1b[38;5;82m"
#define FG_MEDIUM_GREEN "\x1b[38;5;34m"
#define FG_DARK_GREEN   "\x1b[38;5;22m"

/* typedefs ------------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
void MatrixCharacterNoise(uint32_t frame, uint8_t density_scale);
void MatrixCharacterDissolve(uint32_t frame, uint8_t density_scale);

#endif /* __GFX_MATRIX_STREAM_H */