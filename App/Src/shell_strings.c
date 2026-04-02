/**
 ******************************************************************************
 * @file           : shell_strings.c
 * @brief          : String tables and constants for the CLI shell interface,
 * including system information. All user-visible shell text is defined here
 * in one place. To update any terminal output, edit only this file.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
// Project libraries
#include "shell_strings.h"

// STM32 libraries
#include "main.h"

// Standard libraries
#include <stdint.h>

/* Private Variables ---------------------------------------------------------*/
const char prompt[] = "STM32F4>";

// Command & paremeter/topic token text
const char run_command_text[] = "run";
const char help_command_text[] = "help";
const char demo_paremeter_text[] = "demo";

// Run Command Qualifiers token text
const char auto_mode_qualifier_text[] = "/mode=auto";
const char playlist_mode_qualifier_text[] = "/mode=playlist";

// Help Command Qualifiers token text
const char mode_qualifier_text[] = "/mode";
const char scene_qualifier_text[] = "/scene";

// Shell innitialization providing system information during boot
const char *const shell_boot[] = {
    /* [0] NAME      */ "STM32F407VG BIOS v1.0.4",
    /* [1] COPYRIGHT */ "(C) 2026 Taseen ASCII Graphics Demo",
    /* [2] CPU       */ "CPU: ARM Cortex-M4 @ 168MHz (PLL_LOCKED)",
    /* [3] SRAM      */ "SRAM: 128KB OK",
    /* [4] FLASH     */ "FLASH: 1024KB OK",
    /* [5] DMA       */ "DMA Controller... Initialized",
    /* [6] UART      */ "UART2 Terminal... Connected at 921600bps",
    /* [7] TERMINAL  */ "Display Mode... 80x24 ANSI Color",
    /* [8] READY     */ "System is ready...",
    /* [9] HINT      */ "Type 'HELP DEMO' for command usage information.",
};

// Help information for the "HELP DEMO" command, providing an overview of the demo command and its qualifiers
const char *const shell_help_demo[] = {
    "DEMO",
    "",
    "  Invokes the DEMO program showcasing various scenes",
    "  and graphics capabilities of the system.",
    "",
    "  Format:  RUN DEMO [/MODE=type or /SCENE=name]",
    "",
    "Additional information available:",
    "",
    "  /MODE      /SCENE",
};

// Help information for the "HELP RUN" command, providing an overview of the run command and its usage
const char *const shell_help_run[] = {
    "RUN",
    "",
    "  Starts the execution of a specified program.",
    "  Qualifiers may be appended to modify execution behavior.",
    "",
    "  Format:  RUN [program name] [/QUALIFIER=...]",
    "",
    "Additional information available:",
    "",
    "  DEMO",
};

// Help information for the "HELP DEMO /MODE" command, providing details about the mode qualifier for the demo command
// and the valid mode options
const char *const shell_help_subkey_mode[] = {
    "DEMO",
    "  /MODE",
    "",
    "    /Mode=name",
    "    Specifies the playback behavior for the DEMO program.",
    "",
    "    Valid modes are:",
    "      AUTO      Displays every scene sequentially at set intervals.",
    "      PLAYLIST  Plays a curated list of specific scenes back-to-back.",
};

// Help information for the "HELP DEMO /SCENE" command, providing details about the scene qualifier for the demo command
// and how it allows users to launch specific scenes directly from the CLI shell
const char *const shell_help_subkey_scene[] = {
    "DEMO",
    "  /SCENE",
    "",
    "    /SCENE=name",
    "    Specifies the graphics scene to launch immediately, bypassing",
    "    the interactive dashboard menu.",
};

// Shell error messages for different error scenarios in the command parser, providing user-friendly feedback in the CLI
// shell when errors occur
const char *const shell_error[] = {
    /* [0] NONE               */ "",
    /* [1] UNKNOWN_COMMAND    */ "%SYSTEM-E-UNRECOGNIZED, command not found",
    /* [2] MISSING_TOPIC      */ "%HELP-E-NOTOPIC, please specify a help topic (e.g., HELP DEMO)",
    /* [3] UNKNOWN_TOPIC      */ "%HELP-E-UNKNOWNTOPIC, no documentation available for that topic",
    /* [4] UNKNOWN_QUALIFIER  */ "%SYSTEM-E-INVQUAL, unrecognized qualifier in command string",
    /* [5] INVALID_PARAMETER  */ "%SYSTEM-E-INVPARAM, invalid parameter value provided",
    /* [6] UNEXPECTED_ERROR   */ "%SYSTEM-F-ANOMALY, an unexpected shell error occurred",
};

// Calculate the lengths of the string arrays for use in loops and bounds checking when printing the strings in the CLI
// shell
const uint8_t shell_boot_len = sizeof(shell_boot) / sizeof(shell_boot[0]);
const uint8_t shell_help_demo_len = sizeof(shell_help_demo) / sizeof(shell_help_demo[0]);
const uint8_t shell_help_run_len = sizeof(shell_help_run) / sizeof(shell_help_run[0]);
const uint8_t shell_help_subkey_mode_len = sizeof(shell_help_subkey_mode) / sizeof(shell_help_subkey_mode[0]);
const uint8_t shell_help_subkey_scene_len = sizeof(shell_help_subkey_scene) / sizeof(shell_help_subkey_scene[0]);