/**
 ******************************************************************************
 * @file           : doom_font.h
 * @brief          :
 ******************************************************************************
*/

#ifndef __DOOM_FONT_H
#define __DOOM_FONT_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// User includes
#include "gfx_text.h"

/* defines -------------------------------------------------------------------*/
// ASCII printable characters (character code 32-127)
#define ASCII_SPACE          32 // ' '
#define ASCII_SINGLE_QUOTE   39 // '''
#define ASCII_OPEN_PAREN     40 // '('
#define ASCII_CLOSE_PAREN    41 // ')'
#define ASCII_COMMA          44 // ','
#define ASCII_HYPHEN         45 // '-'
#define ASCII_DOT            46 // '.'
#define ASCII_SLASH          47 // '/'
#define ASCII_LESS_THAN      60 // '<'
#define ASCII_GREATER_THAN   62 // '>'
#define ASCII_LETTER_V       86 // 'V'
#define ASCII_BACKSLASH      92 // '\'
#define ASCII_CARET          94 // '^'
#define ASCII_UNDERSCORE     95 // '_'
#define ASCII_BACKTICK       96 // '`'
#define ASCII_PIPE          124 // '|'

/* externs -------------------------------------------------------------------*/
extern const Charachter_Font_t doom_lookup[];
extern const uint16_t font_doom_rle_data[];
extern const Font_t font_doom;

#endif /* __DOOM_FONT_H */
