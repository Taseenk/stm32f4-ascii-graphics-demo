/**
 ******************************************************************************
 * @file           : renderer.h
 * @brief          :
 ******************************************************************************
 */

#ifndef __RENDERER_H
#define __RENDERER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// Standard libraries
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// User includes
#include "console.h"

/* defines -------------------------------------------------------------------*/


/* typedefs ------------------------------------------------------------------*/
typedef struct {
	uint8_t ascii_code;         // The character this entry represents (e.g., 'A', 'a', '!')
	uint8_t start_offset_rle;   // The starting index in the font's raw RLE data array
    uint8_t end_offset_rle;     // The ending index in the font's raw RLE data array
    uint8_t col_width;          // The maximum width of the character in columns
	uint8_t row_height;         // The height of the character in rows
} Charachter_Font_t;

typedef struct {
    const char *name;
    const Charachter_Font_t *lookup_table;
    const size_t table_size; 
    const uint8_t *rle_data;
} Font_t;

/* Private Variables ---------------------------------------------------------*/


/* function prototypes -------------------------------------------------------*/
const Charachter_Font_t *CharachterLookup(UART_HandleTypeDef *huart, char font_char, const Font_t *font);
void DecodeCharachterRle(UART_HandleTypeDef *huart, char *str, const Font_t *font);

#endif /* __RENDERER_H */
