/**
 ******************************************************************************
 * @file           : big_money_nw_font.h
 * @brief          :
 ******************************************************************************
 */

#ifndef __BIG_MONEY_NW_FONT_H
#define __BIG_MONEY_NW_FONT_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// User includes
#include "gfx_text.h"

/* defines -------------------------------------------------------------------*/
// ASCII printable characters (character code 32-127)
#define ASCII_SPACE             32  // ' '
#define ASCII_DOLLAR            36  // '$'
#define ASCII_SLASH             47  // '/'
#define ASCII_LESS_THAN         60 // '<'
#define ASCII_BACKSLASH         92  // '\'
#define ASCII_UNDERSCORE        95  // '_'
#define ASCII_PIPE              124 // '|'

/* externs -------------------------------------------------------------------*/
extern const Charachter_Font_t big_money_nw_lookup[];
extern const uint16_t font_big_money_nw_rle_data[];
extern const Font_t font_big_money_nw;

#endif /* __BIG_MONEY_NW_FONT_H */
