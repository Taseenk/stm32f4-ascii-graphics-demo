/**
 ******************************************************************************
 * @file           : shell_strings.h
 * @brief          : 
 ******************************************************************************
 */

#ifndef __SHELL_STRINGS_H
#define __SHELL_STRINGS_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define RUN_COMMAND_TEXT_LEN    3       // Length of the "run" command text for parsing user input
#define HELP_COMMAND_TEXT_LEN   4       // Length of the "help" command text for parsing user input

/* External variables --------------------------------------------------------*/
// Prompt text for the CLI shell interface
extern const char prompt[];

// Command & parameter/topic token text
extern const char run_command_text[];
extern const char help_command_text[];
extern const char demo_paremeter_text[];

extern const char auto_mode_qualifier_text[];
extern const char playlist_mode_qualifier_text[];

extern const char mode_qualifier_text[];
extern const char scene_qualifier_text[];

// Shell boot sequence strings and help information strings for the CLI shell interface
extern const char *const shell_boot[];
extern const char *const shell_help_demo[];
extern const char *const shell_help_run[];
extern const char *const shell_help_subkey_mode[];
extern const char *const shell_help_subkey_scene[];

// Error message strings for the CLI shell interface
extern const char *const shell_error[];

// Lengths of the shell strings arrays for iterating and printing in the CLI shell interface
extern const uint8_t shell_boot_len;
extern const uint8_t shell_help_demo_len;
extern const uint8_t shell_help_run_len;
extern const uint8_t shell_help_subkey_mode_len;
extern const uint8_t shell_help_subkey_scene_len;

#endif /* __SHELL_STRINGS_H */