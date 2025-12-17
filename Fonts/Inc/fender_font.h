/**
 ******************************************************************************
 * @file           : fender_font.h
 * @brief          :
 ******************************************************************************
*/

#ifndef __FENDER_FONT_H
#define __FENDER_FONT_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// User includes
#include "renderer.h"

/* defines -------------------------------------------------------------------*/
// ASCII printable characters (character code 32-127)
#define ASCII_SPACE          32 // ' '
#define ASCII_PERCENT        37 // '%'
#define ASCII_SINGLE_QUOTE   39 // '''
#define ASCII_OPEN_PAREN     40 // '('
#define ASCII_CLOSE_PAREN    41 // ')'
#define ASCII_COMMA          44 // ','
#define ASCII_HYPHEN         45 // '-'
#define ASCII_DOT            46 // '.'
#define ASCII_SLASH          47 // '/'
#define ASCII_SEMICOLON      59 // ';'
#define ASCII_LESS_THAN      60 // '<'
#define ASCII_GREATER_THAN   62 // '>'
#define ASCII_BACKSLASH      92 // '\'
#define ASCII_BACKTICK       96 // '`'
#define ASCII_OPEN_BRACE    123 // '{'
#define ASCII_PIPE          124 // '|'
#define ASCII_CLOSE_BRACE   125 // '}'

/* externs -------------------------------------------------------------------*/
extern const Charachter_Font_t fender_lookup[];
extern const uint16_t font_fender_rle_data[];
extern const Font_t font_fender;

#endif /* __FENDER_FONT_H */
