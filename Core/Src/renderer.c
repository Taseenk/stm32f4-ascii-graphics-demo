/**
 ******************************************************************************
 * @file           : renderer.c
 * @brief          :
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "renderer.h"

/* Private Variables ---------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
/**
 * @fn const Charachter_Font_t *CharachterLookup(UART_HandleTypeDef *huart, char font_char, const Font_t *font)
 * @brief Searches a font's lookup table for the entry matching the given ASCII character.
 * If the character is not found, a warning is logged via the console, and NULL is returned.
 * @param huart A pointer to the UART_HandleTypeDef structure that contains
 * the configuration information for the specified UART.
 * @param font_char The ASCII character to search for (e.g., 'a', 'b', '1').
 * @param font A pointer to the Font_t structure containing the lookup table and its size.
 * @return A constant pointer to the Charachter_Font_t entry if found, otherwise NULL.
 */
const Charachter_Font_t *CharachterLookup(UART_HandleTypeDef *huart, char font_char, const Font_t *font)
{
	// Iterate through the font's lookup table
	for (uint8_t i = 0; i < font->table_size; i++) {
		// Check if the current entry's ASCII code matches the character
		if (font->lookup_table[i].ascii_code == font_char)
			// Return the adress to the matching entry structure
			return &font->lookup_table[i];
	}

	// Log a warning message that the character was not recognized
	ConsolePrintNewLine(huart, "ERROR: Character was not recognized");

	// If the charachter was not found return NULL
	return NULL;
}

/**
 * @fn void DecodeCharachterRle(UART_HandleTypeDef *huart, char *str, const Font_t *font)
 * @brief Decodes the RLE data for a given character from the specified font
 * and stores the decoded data in a buffer.
 * @param huart A pointer to the UART_HandleTypeDef structure for console logging.
 * @param str A string containing the character to decode (only the first character is used).
 * @param font A pointer to the Font_t structure containing the RLE data and lookup table.
 */
void DecodeCharachterRle(UART_HandleTypeDef *huart, char *str, const Font_t *font)
{
	// Extract the character to be decoded and find the RLE data (start-offset, end-offset,width and height)
	char font_char = str[0];
	const Charachter_Font_t *lookup = CharachterLookup(huart, font_char, font);

	if (lookup == NULL) {
		// Log a warning message if the character was not found in the font's lookup table
		ConsolePrintNewLine(huart, "ERROR: No data was received from the lookup table");

		// Exit the function early since there is no data to decode
		return;
	}

	// Get the RLE data offsets from the lookup entry
	uint8_t start_offset = lookup->start_offset_rle;
	uint8_t end_offset = lookup->end_offset_rle;

	// Font character dimensions used for sizing the output buffer
	uint8_t col = lookup->col_width;
	uint8_t row = lookup->row_height;

	// Prepare a buffer for one row, size temporary set to col + 3 (to fit largest font + \r\n + \0)
	char decoded_row_buffer[13]; 

	// Initializing indexes for RLE data and decoded buffer
	uint8_t rle_index = 0, decoder_index = 0, multiplier = 0;
	char character_code;

	// Outerloop for itterating through the rle font data array
	for (rle_index = start_offset; rle_index < end_offset; rle_index += 2) {
		// Read the multiplier (Count) from the RLE data array
		multiplier = font->rle_data[rle_index];

		// Read the character code from the next byte
		character_code = (char)font->rle_data[rle_index + 1];

		// Write the character code 'multiplier' number of times
		for (int i = 0; i < multiplier; i++) {
			// Write the character code into the decoded buffer
            decoded_row_buffer[decoder_index] = character_code;

            // Increment the decoder index
            decoder_index++;

			// Check if the current row is now full
            if (decoder_index == col) {
                // Terminate and print the row
                decoded_row_buffer[col] = '\r';
                decoded_row_buffer[col + 1] = '\n';
                decoded_row_buffer[col + 2] = '\0'; // Note: Using col for clarity
                ConsolePrint(huart, decoded_row_buffer);

                // Reset the decoder index for the next row
                decoder_index = 0;
            }
		}
	}
}
